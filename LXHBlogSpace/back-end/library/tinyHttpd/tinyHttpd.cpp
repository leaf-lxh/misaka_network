#include "tinyHttpd.h"
#include <regex>
#include <fstream>
#include <iostream>
#include <sstream>
#include <memory>
#include <stdexcept>

#include <unistd.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netdb.h>

#include <syslog.h>

#include "webstring.h"


#define WATERLINE_WRITE_BUFFER 4096
#define SERVER_SIGNATURE "TinyHttpd/0.1a"


std::map<std::string, std::string> RetrieveFromKeyValueFmt(std::string path)
{
	std::fstream file(path, std::fstream::in);
	if (file.good() == false)
	{
		throw std::runtime_error(std::string("Failed to open file: ") + path);
	}
	file.seekg(0, file.end);
	size_t fileLength = file.tellg();
	file.seekg(0, file.beg);

	if (fileLength <= 0)
	{
		return {};
	}
	std::unique_ptr<char[]> buffer(new char[fileLength]());
	file.read(buffer.get(), fileLength);
	file.close();

	std::string fileContent(buffer.get());
	std::regex configFormat("(.+?)=(.+?)\n");
	std::regex commentFormat("([^#]*?)#(.*?)");
	std::sregex_iterator rend;
	std::map<std::string, std::string> settingMap;

	for (auto it = std::sregex_iterator(fileContent.begin(), fileContent.end(), configFormat); it != rend; ++it)
	{
		std::smatch result;
		std::string buffer((*it)[2].str());
		if (std::regex_match(buffer, result, commentFormat))
		{
			settingMap.insert({ webstring::strip((*it)[1].str()), webstring::strip(result[1].str()) });
		}
		else
		{
			settingMap.insert({ webstring::strip((*it)[1].str()), webstring::strip((*it)[2].str()) });
		}
	}

	return settingMap;

}

TinyHttpd::~TinyHttpd()
{
	if (serverProperty.verbose >= VerboseLevel::less)
	{
		std::cout << "Closing server...." << std::endl;
	}
	
	for (auto client : connectedClients)
	{
		close(client.first);
	}
	close(serverProperty.listenfd);
}

std::map<std::string, std::string> TinyHttpd::Init(std::string confPath) noexcept(false)
{
	using namespace std;
	map<string, string> setting = RetrieveFromKeyValueFmt(confPath);
	if (setting.count("port") && setting.count("address"))
	{
		try
		{
			serverProperty.bind = { stoi(setting["port"]), webstring::strip(setting["address"], "\"\'") };
		}
		catch (std::runtime_error e)
		{
			throw runtime_error("Invaild port or address, examine your configuration file.");
		}
	}

	if (setting.count("unixPath") != 0)
	{
		serverProperty.unixBind = webstring::strip(setting["unixPath"], "\"\'");
	}
	else
	{
		serverProperty.unixBind = "/var/run/tinyHttpd/";
	}

	if (setting.count("maxClients") != 0)
	{
		try
		{
			serverProperty.maxClients = stoi(setting["maxClients"]);
			if (serverProperty.maxClients <= 1)
			{
				throw std::runtime_error("?");
			}
		}
		catch (std::runtime_error e)
		{
			throw runtime_error(std::string("Invaild maxClients: ") + setting["maxClients"]);
		}
	}
	else
	{
		serverProperty.maxClients = 1024;
	}
	
	if (setting.count("documentRoot") != 0)
	{
		serverProperty.documentRoot = webstring::strip(setting["documentRoot"], " \"\'");
	}
	else
	{
		serverProperty.documentRoot = "/var/www/html";
	}

	//对于路径末尾是否带斜线的处理。documentRoot末尾应缀上斜线
	if (serverProperty.documentRoot[serverProperty.documentRoot.length() - 1] != '/')
	{
		serverProperty.documentRoot += '/';
	}

	if (setting.count("verboseLevel") != 0)
	{
		try
		{
			switch (stoi(setting["verboseLevel"]))
			{
			case 3:
				serverProperty.verbose = VerboseLevel::full;
				break;
			case 2:
				serverProperty.verbose = VerboseLevel::essential;
				break;
			case 1:
				serverProperty.verbose = VerboseLevel::less;
				break;
			default:
				serverProperty.verbose = VerboseLevel::silence;
				break;
			}
		}
		catch (std::runtime_error e)
		{
			throw runtime_error("Invaild verbose level.");
		}
	}
	else
	{
		serverProperty.verbose = VerboseLevel::silence;
	}
	
	if (setting.count("KeepAliveTimeout") != 0)
	{
		try
		{
			serverProperty.timeout = stoi(setting["KeepAliveTimeout"]);
		}
		catch (runtime_error e)
		{
			throw runtime_error("Invaild timeout value.");
		}
	}
	else
	{
		serverProperty.timeout = 5;
	}

	if (setting.count("KeepAliveMaxRequestNum") != 0)
	{
		try
		{
			serverProperty.maxRequestsNum = stoi(setting["KeepAliveMaxRequestNum"]);
		}
		catch (runtime_error e)
		{
			throw runtime_error("Invaild timeout value.");
		}
	}
	else
	{
		serverProperty.maxRequestsNum = 60;
	}

	return setting;
}

void TinyHttpd::LoadMIME(std::string path) noexcept
{
	using namespace std;
	fstream mimeFile(path, fstream::in);
	string textLine;
	regex confFmt("(\\S+?)\\s+?(\\S.*?);");
	regex typeFmt("([^ ]+) {0,}");
	smatch conf;
	while (getline(mimeFile, textLine))
	{
		if (regex_search(textLine, conf, confFmt))
		{
			string typeText = conf[2].str();
			sregex_iterator endit;
			for (auto it = sregex_iterator(typeText.begin(), typeText.end(), typeFmt); it != endit; ++it)
			{
				serverProperty.mimeMap.insert({ it->operator[](1).str(), conf[1].str() });
			}
		}
	}
}

bool TinyHttpd::AddRoutePath(std::string path) noexcept
{
	return serverProperty.routeTable.insert(path).second;
}

void TinyHttpd::DumpProperty() noexcept
{
	using namespace std;
	cout << "listenfd: " << serverProperty.listenfd << endl;
	cout << "bind address: " << serverProperty.bind.second << "->" << serverProperty.bind.first << endl;
	cout << "unix bind address: " << serverProperty.unixBind << endl;
	cout << "documentRoot: " << serverProperty.documentRoot << endl;
	cout << "maxClients: " << serverProperty.maxClients << endl;
	cout << "route table: " << endl;
	for (auto path : serverProperty.routeTable)
	{
		cout << "\t" << path << endl;
	}

	cout << "Loaded mime types: " << endl;
	for (auto type : serverProperty.mimeMap)
	{
		cout << type.first << ": " << type.second << endl;
	}

	cout << "dump finished" << endl;
}

void TinyHttpd::StartListen() noexcept(false)
{
	serverProperty.listenfd = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in serverInfo = {};
	serverInfo.sin_family = AF_INET;
	serverInfo.sin_port = htons(serverProperty.bind.first);
	if (inet_pton(AF_INET, serverProperty.bind.second.c_str(), &serverInfo.sin_addr.s_addr) <= 0)
	{
		throw std::runtime_error(std::string("invaild bind address: ") + serverProperty.bind.second);
	}

	if (bind(serverProperty.listenfd, (sockaddr*)& serverInfo, sizeof(sockaddr_in)) != 0)
	{
		throw std::runtime_error(std::string("unable to bind address for: ") + serverProperty.bind.second + std::string(": ") + std::to_string(serverProperty.bind.first));
	}
	
	if (listen(serverProperty.listenfd, serverProperty.maxClients) != 0)
	{
		throw std::runtime_error("listen error");
	}

	/*
	int flags = fcntl(serverProperty.listenfd, F_GETFL, 0);
	flags |= O_NONBLOCK;
	fcntl(serverProperty.listenfd, F_SETFL, flags);
	*/
	
}

void TinyHttpd::StartHandleRequest() noexcept
{
	int epollfd = epoll_create(serverProperty.maxClients);
	AddEvent(epollfd, serverProperty.listenfd, EPOLLIN);
	std::unique_ptr<struct epoll_event[]> events(new struct epoll_event[serverProperty.maxClients]());
	std::unique_ptr<char[]> receiveBuffer(new char[4096]());

	int readyEvents = 0;
	int clientfd;
	while (true)
	{
		//这里应该使用异步IO来读取文件，因为fstream无法支持异步IO，无法与epoll进行结合
		readyEvents = epoll_wait(epollfd, events.get(), serverProperty.maxClients, -1);
		for (int index = 0; index < readyEvents; ++index)
		{
			clientfd = events[index].data.fd;

			if (clientfd == serverProperty.listenfd)
			{
				ClientProperty clientInfo = {};
				clientInfo.lastAlive = time(nullptr);

				socklen_t length = sizeof(sockaddr_in);
				clientfd = accept(clientfd, (sockaddr*)&clientInfo.clientInfo, &length);
				if (clientfd > 0)
				{
					clientInfo.fd = clientfd;
					connectedClients.insert({ clientfd, clientInfo });
					AddEvent(epollfd, clientfd, EPOLLIN);
				}
				else if (serverProperty.verbose >= VerboseLevel::full)
				{
					std::cout << errno << std::endl;
				}
				continue;
			}
			

			if (events[index].events & EPOLLIN)
			{
				if (connectedClients[clientfd].readShutdown == true)
				{
					continue;
				}
				
				ssize_t totalLength = 0;
				ssize_t length = 0;
				do
				{
					memset(receiveBuffer.get(), 0, 4096);
					length = recv(clientfd, receiveBuffer.get(), 4096, MSG_DONTWAIT);
					if (length > 0)
					{
						//对于字节流，应当使用append方法将数据复制过去。而不是直接append一个const char*指针，因为这样做可能会被追加数据中的\0截断
						connectedClients[clientfd].readBuffer.append(receiveBuffer.get(), length);
						totalLength += length;
					}
				}
				while(length > 0);

				if (length == 0)
				{
					DeleteEvent(epollfd, clientfd, EPOLLIN|EPOLLOUT| EPOLLRDHUP);
					CloseConnection(clientfd);
					continue;
				}
				else
				{
					if (totalLength > 0)
					{
						HTTPProfiler(clientfd);
						connectedClients[clientfd].lastAlive = time(nullptr);
					}
					continue;
				}
			}
			
			if (events[index].events & EPOLLOUT)
			{
				if (connectedClients[clientfd].writeBuffer.length() > 0)
				{
					PushData(clientfd);

					//尝试读取所请求大文件的一部分数据到writeBuffer中
					if ((connectedClients[clientfd].writeBuffer.length() < WATERLINE_WRITE_BUFFER) && (connectedClients[clientfd].aviliableFileBytesNum > 0))
					{
						try
						{
							ReadFileStreamToBuffer(clientfd);
						}
						catch (std::runtime_error e)
						{
							DeleteEvent(epollfd, clientfd, EPOLLIN | EPOLLOUT | EPOLLRDHUP);
							CloseConnection(events[index].data.fd);
							continue;
						}
					}
				}
			}

			if (events[index].events & EPOLLRDHUP)
			{
				DeleteEvent(epollfd, clientfd, EPOLLIN | EPOLLOUT | EPOLLRDHUP);
				CloseConnection(events[index].data.fd);
			}
		}


		//KeepAlive timeout处理
		time_t currentTime = time(nullptr);
		
		std::set<int> willDeleted;
		for (auto client = connectedClients.begin(); client != connectedClients.end(); ++client)
		{
			//如果超时，读关闭，发送完数据再关闭
			if ((client->second.keepAlive == true) && ((currentTime - client->second.lastAlive) > serverProperty.timeout))
			{
				shutdown(client->first, SHUT_RD);
				client->second.readShutdown = true;
			}

			//如果要立即关闭，则加入到关闭队列中
			if (client->second.fullShutdown == true)
			{
				willDeleted.insert(client->first);
				continue;
			}

			//如果readShutdown启用
			if (client->second.readShutdown == true)
			{
				if (client->second.writeBuffer.length() > 0)
				{
					//如果还有要write的数据，继续监听写的状态
					ModifyEvent(epollfd, client->first, EPOLLOUT | EPOLLRDHUP);
				}
				else
				{
					//发送完数据，关闭当前连接
					willDeleted.insert(client->first);
					continue;
				}
			}
			else
			{
				if (client->second.writeBuffer.length() > 0)
				{
					ModifyEvent(epollfd, client->first, EPOLLIN | EPOLLOUT | EPOLLRDHUP);
				}
				else
				{
					ModifyEvent(epollfd, client->first, EPOLLIN | EPOLLRDHUP);
				}
			}
		}

		//关闭连接
		for (auto fd : willDeleted)
		{
			CloseConnection(fd);
		}
	}
}

void TinyHttpd::PushData(int fd) noexcept
{
	std::size_t sendLength = (4096 > connectedClients[fd].writeBuffer.length()) ? connectedClients[fd].writeBuffer.length() : 4096;
	ssize_t sended = send(fd, connectedClients[fd].writeBuffer.c_str(), sendLength, MSG_DONTWAIT);
	if (sended > 0)
	{
		connectedClients[fd].writeBuffer = connectedClients[fd].writeBuffer.substr(sended);
	}
}

void TinyHttpd::ReadFileStreamToBuffer(int fd) noexcept(false)
{
	//判断是否能读
	if (connectedClients[fd].file->good() == false)
	{
		throw std::runtime_error("connectedClients[fd].file->good() == false");
	}

	std::unique_ptr<char[]> buffer(new char[4096]());
	std::streamsize readLength = (4096 > connectedClients[fd].aviliableFileBytesNum) ? connectedClients[fd].aviliableFileBytesNum : 4096;
	std::streamsize readedLength = connectedClients[fd].file->readsome(buffer.get(), readLength);
	if (readedLength > 0)
	{
		connectedClients[fd].writeBuffer.append(buffer.get(), readedLength);
		connectedClients[fd].aviliableFileBytesNum = connectedClients[fd].aviliableFileBytesNum - readedLength;
	}
}

void TinyHttpd::HTTPProfiler(int fd) noexcept
{
	using namespace std;
	regex requestLineFmt("^([A-Z]+?) (\\S+?) HTTP/(.+?)\r\n");
	regex requestHeaderFmt("(.+?):(.*?)\r\n");

	HTTPPacket::HTTPRequestPacket packet;
	smatch requestLine;
	smatch requestHeaders;

	//查找报文头(header)与报文体(body)的分割
	size_t requestHeaderEnd = connectedClients[fd].readBuffer.find("\r\n\r\n");
	if (requestHeaderEnd != string::npos)
	{
		//判断该数据是否完整匹配一个报文请求行(request line)的开始
		if (!regex_search(connectedClients[fd].readBuffer,  requestLine, requestLineFmt))
		{
			return;
		}

		sregex_iterator itend;
		for (sregex_iterator it(connectedClients[fd].readBuffer.cbegin(), connectedClients[fd].readBuffer.cbegin() + requestHeaderEnd + 2, requestHeaderFmt); it != itend; ++it)
		{
			packet.requestHeaders.insert({ webstring::strip(it->operator[](1).str()), webstring::strip(it->operator[](2).str()) });
		}

		//如果没有host头，则为bad request
		if (packet.IsHeaderExist("host", true) == 0)
		{
			RaiseHTPPError(fd, 400);
			return;
		}

		//判断是否有body
		size_t contentLength = 0;
		try
		{
			std::string contentLengthStr = packet.GetContentLength();
			if (contentLengthStr != "")
			{
				contentLength = stoul(contentLengthStr);
			}
		}
		catch (...)
		{
			RaiseHTPPError(fd, 400);
			return;
		}
		
		//判断当前剩余的数据长度是否满足content-length的长度
		if (connectedClients[fd].readBuffer.length() - (requestHeaderEnd + 4) >= contentLength)
		{
			packet.method = requestLine[1].str();
			size_t paramStart = requestLine[2].str().find("?");
			if (paramStart != std::string::npos)
			{
				packet.requestPath = requestLine[2].str().substr(0, paramStart);
				packet.requestParam = requestLine[2].str().substr(paramStart + 1);
				packet.fullURL = requestLine[2].str();
			}
			else
			{
				packet.requestPath = requestLine[2].str();
				packet.fullURL = requestLine[2].str();
			}

			packet.httpVersion = requestLine[3].str();

			packet.body = connectedClients[fd].readBuffer.substr(requestHeaderEnd + 4, requestHeaderEnd + 4 + contentLength);
			connectedClients[fd].readBuffer = connectedClients[fd].readBuffer.substr(requestHeaderEnd + 4 + contentLength);
			this->HTTPPacketHandler(fd, packet);
		}
	}
}

void TinyHttpd::CloseConnection(int fd)
{
	close(fd);
	connectedClients.erase(fd);
}

void TinyHttpd::HTTPPacketHandler(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept
{
	HTTPResponsePacket response;
	response.code = "200 OK";
	response.version = "HTTP/1.1";
	response.responseHeaders.insert({ "Server", SERVER_SIGNATURE });
	response.responseHeaders.insert({ "Content-Encoding", "plain" });
	std::string path;
	if (request.method == "GET")
	{
		
		if (request.requestPath == "/")
		{
			path = serverProperty.documentRoot + "index.html";
		}
		else
		{
			//实际上Linux系统会将多个斜线视为一个斜线...
			path = serverProperty.documentRoot + webstring::LeftStrip(request.requestPath, " /");
		}


		path = webstring::URLdecode(path);

		switch (IsAccessableFile(path))
		{
		case FileAccessableStat::normalFile:
			break;
		case FileAccessableStat::directory:
			if (IsAccessableFile(path + "/index.html") == FileAccessableStat::normalFile)
			{
				path += "/index.html";
				break;
			}
			else
			{
				RaiseHTPPError(clientfd, 403);
				return;
			}
		case FileAccessableStat::forbiden:
			RaiseHTPPError(clientfd, 403);
			return;
		case FileAccessableStat::noneExist:
			RaiseHTPPError(clientfd, 404);
			return;
		case FileAccessableStat::unknow:
		default:
			RaiseHTPPError(clientfd, 500);
			return;
		}

		std::streamsize filesize = GetFileLength(path);
		response.responseHeaders.insert({ "Content-Type", GetResponseType(path) });
		response.responseHeaders.insert({ "Content-Length", std::to_string(filesize) });
		if (filesize > 4096)
		{
			//如果文件过大应该使用延时读取,避免影响其他请求的响应。此时该连接只能响应这个读文件请求
			response.responseHeaders.insert({ "Connection", "Closed" });
			connectedClients[clientfd].file.reset(new std::fstream(path, std::fstream::in|std::fstream::binary));
			connectedClients[clientfd].aviliableFileBytesNum = filesize;
			connectedClients[clientfd].readShutdown = true;
			connectedClients[clientfd].writeBuffer += ResponseToString(response);
		}
		else
		{
			//如果所请求的文件较小，几乎可以瞬间读取完毕，不影响正常执行响应，则可以使用该函数来读取文件内容
			response.body += GetFileContent(path);
			std::stringstream param;
			param << std::noskipws;
			param << "timeout=" << serverProperty.timeout << ", max=" << serverProperty.maxRequestsNum;
			response.responseHeaders.insert({ "Connection", "Keep-Alive" });
			response.responseHeaders.insert({ "Keep-Alive", param.str() });
			connectedClients[clientfd].writeBuffer += ResponseToString(response);
		}
	}
	else
	{
		RaiseHTPPError(clientfd, 501);
	}

}

std::string TinyHttpd::ResponseToString(HTTPResponsePacket response) noexcept
{
	std::stringstream result;
	result << std::noskipws;
	result << response.version << " " << response.code << "\r\n";
	for (auto header : response.responseHeaders)
	{
		result << header.first << ": " << header.second << "\r\n";
	}
	result << "\r\n";
	result << response.body;

	std::string str = result.str();
 	return str;
}

std::string TinyHttpd::GetFileContent(std::string &path) noexcept(false)
{
	using namespace std;

	fstream file(path, fstream::in|fstream::binary);
	if (file.good() == false)
	{
		throw runtime_error("500");
	}
	file.seekg(0, fstream::end);
	streamsize fileSize = file.tellg();
	file.seekg(0, fstream::beg);

	if (fileSize > 0)
	{
		unique_ptr<char[]> buffer(new char[fileSize+1]());
		file.read(buffer.get(), fileSize);
		return string(buffer.get(), fileSize);
	}
	else
	{
		return "";
	}

}

void TinyHttpd::AddEvent(int epollfd, int fd, int flags) noexcept
{
	struct epoll_event event = {};
	event.events = flags;
	event.data.fd = fd;

	epoll_ctl(epollfd, EPOLL_CTL_ADD , fd, &event);
	if (serverProperty.verbose >= VerboseLevel::full)
	{
		std::cout << "addevent: " << fd << std::endl;
	}
}


void TinyHttpd::ModifyEvent(int epollfd, int fd, int flags) noexcept
{
	struct epoll_event event = {};
	event.events = flags;
	event.data.fd = fd;

	epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event);
	if (serverProperty.verbose >= VerboseLevel::full)
	{
		std::cout << "modifyEnvent: " << fd << std::endl;
	}
}

void TinyHttpd::DeleteEvent(int epollfd, int fd, int flags) noexcept
{
	struct epoll_event event = {};
	event.events = flags;
	event.data.fd = fd;

	epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &event);
	if (serverProperty.verbose >= VerboseLevel::full)
	{
		std::cout << "delevent: " << fd << std::endl;
	}
}

void TinyHttpd::RaiseHTPPError(int fd, int code, std::string additional) noexcept
{
	HTTPResponsePacket response;
	response.code = std::to_string(code);
	response.version = "HTTP/1.1";
	response.responseHeaders.insert({ "Server", SERVER_SIGNATURE });

	std::stringstream page;
	page << std::noskipws;
	switch (code)
	{
	case 400:
		response.code += " Bad Request";
		page << "<h1>Bad Request</h1>\n";
		page << "<p>" << additional << "</p>\n";
		page << "<hr>\n";
		page << SERVER_SIGNATURE;
		response.body = page.str();
		break;
	case 403:
		response.code += " Forbidden";
		page << "<h1>Forbidden</h1>\n";
		page << "<p>" << additional << "</p>\n";
		page << "<hr>\n";
		page << SERVER_SIGNATURE;
		response.body = page.str();
		break;
	case 404:
		response.code += " Not Found";
		page << "<h1>Not Found</h1>\n";
		page << "<p>" << additional << "</p>\n";
		page << "<hr>\n";
		page << SERVER_SIGNATURE;
		response.body = page.str();
		break;
	case 405:
		response.code += " Method Not Allowed";
		page << "<h1>Method Not Allowed</h1>\n";
		page << "<p>" << additional << "</p>\n";
		page << "<hr>\n";
		page << SERVER_SIGNATURE;
		response.body = page.str();
		break;
	case 500:
		response.code += " Internal Server Error";
		page << "<h1>Internal Server Error</h1>\n";
		page << "<p>" << additional << "</p>\n";
		page << "<hr>\n";
		page << SERVER_SIGNATURE;
		response.body = page.str();
		break;
	case 501:
		response.code += "Not Implemented";
		page << "<h1>Not Implemented</h1>\n";
		page << "<p>" << additional << "</p>\n";
		page << "<hr>\n";
		page << SERVER_SIGNATURE;
		response.body = page.str();
		break;
	default:
		response.code += " Undefined";
		page << "<h1>Undefined Error Code</h1>\n";
		page << "<p>" << additional << "</p>\n";
		page << "<hr>\n";
		page << SERVER_SIGNATURE;
		response.body = page.str();
		break;
	}

	response.responseHeaders.insert({ "Content-Length", std::to_string(response.body.length())});
	response.responseHeaders.insert({ "Content-Type", "text/html" });
	connectedClients[fd].writeBuffer = ResponseToString(response);
	connectedClients[fd].readShutdown = true;
}


std::string TinyHttpd::GetResponseType(std::string path) noexcept
{
	std::string fileType = path.substr(path.find_last_of(".") + 1);
	if (serverProperty.mimeMap.count(fileType))
	{
		return serverProperty.mimeMap[fileType];
	}
	else
	{
		return "application/octet-stream";
	}
}

TinyHttpd::FileAccessableStat TinyHttpd::IsAccessableFile(std::string path) noexcept
{
	using namespace std;
	if (access(path.c_str(), F_OK | R_OK) == -1)
	{
		if (errno == EACCES)
		{
			return FileAccessableStat::forbiden;
		}
		else if (errno == ENOENT)
		{
			return FileAccessableStat::noneExist;
		}
		else
		{
			return FileAccessableStat::unknow;
		}
	}

	//判断是否是目录
	FILE* fp = fopen(path.c_str(), "r");
	struct stat fileInfo;
	fstat(fileno(fp), &fileInfo);
	fclose(fp);

	if (!S_ISREG(fileInfo.st_mode))
	{
		return FileAccessableStat::directory;
	}
	else
	{
		return FileAccessableStat::normalFile;
	}
	

}

std::streamsize TinyHttpd::GetFileLength(std::string path) noexcept(false)
{
	using namespace std;
	if (IsAccessableFile(path) == FileAccessableStat::normalFile)
	{
		fstream file(path, fstream::in | fstream::binary);
		file.seekg(0, fstream::end);
		streamsize fileSize = file.tellg();
		return fileSize;
	}
	else
	{
		throw runtime_error("Unable to aquire the file's size.");
	}
}

void TinyHttpd::LogResponse(int clientfd, HTTPPacket::HTTPRequestPacket request, HTTPPacket::ResponseCode rcode) noexcept
{
	using namespace std;
	unique_ptr<char[]> strBuffer(new char[INET_ADDRSTRLEN + 1]());
	inet_ntop(AF_INET, &connectedClients[clientfd].clientInfo.sin_addr.s_addr, strBuffer.get(), INET_ADDRSTRLEN);


	stringstream ss;
	ss << skipws;
	ss << "requester ip: " << strBuffer.get() << " -> " << (int)rcode << " " << request.method << " " << request.fullURL << " User-Agent: " << request.GetUserAgent() << " X-Real-IP: " << request.GetRealIP();

	syslog(LOG_INFO | LOG_USER, "%s", ss.str().c_str());
}

void TinyHttpd::LogRequestError(int clientfd, HTTPPacket::HTTPRequestPacket request, std::string message) noexcept
{
	using namespace std;
	unique_ptr<char[]> strBuffer(new char[INET_ADDRSTRLEN + 1]());
	inet_ntop(AF_INET, &connectedClients[clientfd].clientInfo.sin_addr.s_addr, strBuffer.get(), INET_ADDRSTRLEN);


	stringstream ss;
	ss << skipws;
	ss << message << "; requester ip: " << strBuffer.get() << " -> " << request.method << " " << request.fullURL << " User-Agent: " << request.GetUserAgent() << " X-Real-IP: " << request.GetRealIP();

	syslog(LOG_INFO | LOG_USER, "%s", ss.str().c_str());
}