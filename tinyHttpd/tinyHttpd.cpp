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

#define WATERLINE_WRITE_BUFFER 4096

std::string strip(const std::string& str, const std::string chr = " ")
{
	std::string result(str);
	bool found = false;
	while (true)
	{
		found = false;
		for (auto c = chr.cbegin(); (c != chr.cend()) && (found == false); ++c)
		{
			auto start = result.find_first_not_of(*c);

			if ((start != result.npos) && (start != 0))
			{
				/*如果从左开始能找到非目标字符，且位置不为0*/
				result = result.substr(start);
				found = true;
			}


			auto stop = result.find_last_not_of(*c);
			/*如果从右边开始能找到非目标字符，且位置不为最后*/
			if ((stop != result.npos) && (stop != result.length() - 1))
			{
				result = result.substr(0, stop + 1);
				found = true;
			}
		}

		if (found == false)
		{
			break;
		}
	}

	return result;
}

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
			settingMap.insert({ strip((*it)[1].str()), strip(result[1].str()) });
		}
		else
		{
			settingMap.insert({ strip((*it)[1].str()), strip((*it)[2].str()) });
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
	regex requestLineFmt("([A-Z]+?) (\S+?) HTTP/(.+?)\r\n");
	regex requestHeaderFmt("([^:]+?):(.*?)\r\n");

	smatch requestLine;
	smatch requestHeaders;
	if (connectedClients[fd].readBuffer.find("\r\n\r\n") != string::npos)
	{
		if (!regex_match(connectedClients[fd].readBuffer,  requestLine, requestLineFmt))
		{
			connectedClients[fd].writeShutdown = true;
			return;
		}
		//iter
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