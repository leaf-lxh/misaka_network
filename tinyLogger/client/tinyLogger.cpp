#include "tinyLogger.h"

#include <sstream>
#include <ctime>
#include <memory>
#include <regex>

#include <unistd.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <netdb.h>
#include <errno.h>

TinyLoggerClient::~TinyLoggerClient()
{
	close(connectionfd);
}

TinyLoggerClient::Response TinyLoggerClient::Send(std::string logText, std::string timefmt) noexcept(false)
{

	// 准备日志报文
	std::unique_ptr<char[]> timestr(new char[256]());
	
	time_t currentTime = time(nullptr);
	struct tm *timeInfo = localtime(&currentTime);	
	strftime(timestr.get(), 256, timefmt.c_str(), timeInfo);

	std::string log(timestr.get());
	log += logText + "\n";

	std::stringstream msg;
	
	msg << "version: " << TINYLOGGER_CLIENT_VERSION << "\r\n";
	msg << "app_name: " << appName << "\r\n";
	msg << "app_key: " << authKey << "\r\n";
	msg << "data_length: " << log.length()<< "\r\n";
	msg << "\r\n";
	msg << log;
	
	log = msg.str();

	//发送日志报文至服务器
	send(connectionfd, log.c_str(), log.length(), 0);

	std::unique_ptr<char[]> responsePtr(new char[1024]());
	int i = 0;
	int rlength = 0;

	int epollfd = epoll_create(1);
	struct epoll_event event = {};
	event.events = EPOLLIN;
	event.data.fd = connectionfd;

	epoll_ctl(epollfd, EPOLL_CTL_ADD, connectionfd, &event);

	struct epoll_event received_event;

	int state = epoll_wait(epollfd, &received_event, 1, 2);
	if (state == -1)
	{
		throw std::runtime_error("Connection error");
	}
	else if (state == 0)
	{
		throw std::runtime_error("Receiving timeout");
	}

	rlength = recv(connectionfd, responsePtr.get(), 1023, 0);
	if (rlength == 0)
	{
		close(connectionfd);

		throw std::runtime_error("Connection Closed");
	}

	epoll_ctl(epollfd, EPOLL_CTL_DEL, connectionfd, &event);
	close(epollfd);

	//返回报文分析

	std::string response(responsePtr.get());
	struct Response rmsg;
	std::map<std::string, std::string> headers = ProtocolParser(response.substr(0, response.find("\r\n\r\n") + 2));

	int contentLength = 0;
	try
	{
		contentLength = std::stoi(headers["data_length"]);
	}
	catch (...)
	{
		throw std::runtime_error("Corrupted data length.");
	}

	rmsg.server = headers["server"];

	std::string responseBody = response.substr(response.find("\r\n\r\n") + 4, contentLength);
	try 
	{ 
		rmsg.code = std::stoi(responseBody.substr(0, 3));
	}
	catch (...)
	{
		throw std::runtime_error("Unable to get a vaild response code.");
	}

	if (responseBody.length() <= 4)
	{
		rmsg.msg = "";
	}
	else
	{
		rmsg.msg = responseBody.substr(4);
	}
	return rmsg;
}


bool TinyLoggerClient::Connect() noexcept(false)
{
	connectionfd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in serverInfo = {};
	serverInfo.sin_family = AF_INET;
	serverInfo.sin_port = htons(remote.second);
	
	std::regex format("[0-9]{1,3}.[0-9]{1,3}.[0-9]{1,3}");
	std::smatch result;
	if (std::regex_match(remote.first, result, format) == true)
	{
		inet_pton(AF_INET, remote.first.c_str(), &serverInfo.sin_addr.s_addr);
	}
	else
	{
		struct hostent *hostInfo = gethostbyname(remote.first.c_str());
		if (hostInfo->h_addr_list[0] == NULL)
		{
			throw std::runtime_error(std::string("Unable to resolve host: ") + remote.first);
		}
		std::unique_ptr<char[]> address(new char[INET_ADDRSTRLEN + 1]());
		inet_ntop(AF_INET, hostInfo->h_addr,  address.get(), INET_ADDRSTRLEN);
		inet_pton(AF_INET, address.get(), &serverInfo.sin_addr.s_addr);
	}

	if (connect(connectionfd, (sockaddr *)&serverInfo, sizeof(sockaddr_in)))
	{
		
		throw std::runtime_error(std::to_string(errno) + " Failed to connect to server.");
	}

	return true;
}

std::map<std::string, std::string> TinyLoggerClient::ProtocolParser(const std::string headersBytes)
{
	std::map<std::string, std::string> headers;
	size_t startPos = 0;
	size_t endPos = 0;
	for (endPos = headersBytes.find("\r\n"); endPos != headersBytes.npos; endPos = headersBytes.find("\r\n", endPos + 1))
	{
		std::string header(headersBytes.substr(startPos, endPos - startPos));
		startPos = endPos + 2;

		size_t splitPos = header.find(": ");
		if (splitPos == header.npos) continue;

		std::string key(header, 0, splitPos);
		std::string value(header, splitPos + 2, endPos - (splitPos + 2));

		headers[key] = value;
	}
	return headers;
}
