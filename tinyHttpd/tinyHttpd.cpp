#include "tinyHttpd.h"
#include <regex>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>

#include <unistd.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#include "webstring.h"

#define WATERLINE_WRITE_BUFFER 4096



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

	if (fileLength == 0)
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


void TinyHttpd::Init(std::string confPath) noexcept(false)
{
	using namespace std;
	map<string, string> setting = RetrieveFromKeyValueFmt(confPath);
	if (setting.count("port") && setting.count("address"))
	{
		serverProperty.bind = { stoi(setting["port"]), setting["address"] };
	}

	if (setting.count("unixPath") != 0)
	{
		serverProperty.unixBind = setting["unixPath"];
	}
	else
	{
		serverProperty.unixBind = "/var/run/tinyHttpd/";
	}

	if (setting.count("maxClient") != 0)
	{
		serverProperty.maxClients = stoi(setting["maxClient"]);
	}
	else
	{
		serverProperty.maxClients = 1024;
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
	cout << "route table: " << endl;
	for (auto path : serverProperty.routeTable)
	{
		cout << "\t" << path << endl;
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
					connectedClients.insert({ clientfd, clientInfo });
				}
				continue;
			}

			if (events[index].events == EPOLLIN)
			{
				int length = recv(clientfd, receiveBuffer.get(), 4096, MSG_DONTWAIT);
				if (length > 0)
				{
					connectedClients[clientfd].readBuffer += receiveBuffer.get();
					HTTPProfiler(clientfd);
					connectedClients[clientfd].lastAlive = time(nullptr);
				}
				else if (length == 0)
				{
					CloseConnection(clientfd);
				}
				else
				{
					continue;
				}
			}
			
			if (events[index].events == EPOLLOUT)
			{
				if (connectedClients[clientfd].writeBuffer.length() > 0)
				{
					PushData(clientfd);
					if ((connectedClients[clientfd].writeBuffer.length() < WATERLINE_WRITE_BUFFER) && (connectedClients[clientfd].aviliableFileBytesNum > 0))
					{
						ReadFile(clientfd);
					}
				}
			}
		}

		time_t currentTime = time(nullptr);
		
		for (auto client = connectedClients.begin(); client != connectedClients.end(); ++client)
		{
			if ((currentTime - client->second.lastAlive < 60) || (client->second.fullShutdown == false))
			{
				if (client->second.writeBuffer.length() > 0)
				{
					ModifyEvent(epollfd, client->first, EPOLLIN | EPOLLOUT);
				}
				else
				{
					ModifyEvent(epollfd, client->first, EPOLLIN);
				}
				continue;
			}

			close(client->first);
			//由于map关联容器的特性，在循环中删除某个元素后，其后面的元素迭代器仍有效。访问下一个元素只需要将当前迭代器进行递增操作一次。这边用范围for也是可以的
			connectedClients.erase(client);

		}
	}
}

void TinyHttpd::PushData(int fd)
{
	int sended = send(fd, connectedClients[fd].writeBuffer.c_str(), 4096, MSG_DONTWAIT);
	if (sended > 0)
	{
		connectedClients[fd].writeBuffer = connectedClients[fd].writeBuffer.substr(sended);
	}
}

void TinyHttpd::ReadFile(int fd)
{
	std::unique_ptr<char[]> buffer(new char[4096]());
	std::streamsize readedLength = connectedClients[fd].file->readsome(buffer.get(), 4096);
	if (readedLength > 0)
	{
		connectedClients[fd].writeBuffer.append(buffer.get(), readedLength);
	}
}

void TinyHttpd::HTTPProfiler(int fd)
{
	using namespace std;
	regex requestLineFmt("^([A-Z]+?) (\S+?) HTTP/(.+?)\r\n");
	regex requestHeaderFmt("([^:]+?):(.*?)\r\n");

	HTTPRequestPacket packet;
	smatch requestLine;
	smatch requestHeaders;
	size_t requestHeaderEnd = connectedClients[fd].readBuffer.find("\r\n\r\n");
	if (requestHeaderEnd != string::npos)
	{
		if (!regex_search(connectedClients[fd].readBuffer,  requestLine, requestLineFmt))
		{
			connectedClients[fd].writeShutdown = true;
			return;
		}

		sregex_iterator itend;
		for (sregex_iterator it(connectedClients[fd].readBuffer.cbegin(), connectedClients[fd].readBuffer.cbegin() + requestHeaderEnd + 2, requestHeaderFmt); it != itend; ++it)
		{
			packet.requestHeaders.insert({ webstring::tolower(it->operator[](1).str()), it->operator[](2).str() });
		}

		size_t contentLength = 0;
		if (packet.requestHeaders.count("content-length") > 0)
		{
			try
			{
				//取第一个content-length
				contentLength = stoul(packet.requestHeaders.find("content-length")->second);
			}
			catch (...)
			{
				Raise400(fd);
			}
		}

		if (connectedClients[fd].readBuffer.length() - (requestHeaderEnd + 4) >= contentLength)
		{
			packet.method = requestLine[1].str();
			packet.requestPath = requestLine[2].str();
			packet.version = requestLine[3].str();

			packet.body = connectedClients[fd].readBuffer.substr(requestHeaderEnd + 4, requestHeaderEnd + 4 + contentLength);
			connectedClients[fd].readBuffer = connectedClients[fd].readBuffer.substr(requestHeaderEnd + 4 + contentLength);
			this->HTTPPacketHandler(packet);
		}
	}
}

void TinyHttpd::CloseConnection(int fd)
{
	close(fd);
	connectedClients.erase(fd);
}

std::string TinyHttpd::HTTPPacketHandler(HTTPRequestPacket request)
{
	return "";
}

void TinyHttpd::AddEvent(int epollfd, int fd, int flags) noexcept
{
	struct epoll_event event = {};
	event.events = flags;
	event.data.fd = fd;

	epoll_ctl(epollfd, EPOLL_CTL_ADD , fd, &event);
}


void TinyHttpd::ModifyEvent(int epollfd, int fd, int flags) noexcept
{
	struct epoll_event event = {};
	event.events = flags;
	event.data.fd = fd;

	epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event);
}

void TinyHttpd::DeleteEvent(int epollfd, int fd, int flags) noexcept
{
	struct epoll_event event = {};
	event.events = flags;
	event.data.fd = fd;

	epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &event);
}