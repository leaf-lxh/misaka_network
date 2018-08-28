#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "HTTPRequest.h"

#include <regex>
#include <sstream>


/************************************************************************************************************************
*进行HTTP请求。不支持HTTPS
*参数：requestURL              |请求的URL，应严格按照格式http://host/directory
*      additionalHeaders       |附加的Headers //应为additionalData添加Content-type和Content-encoding,每条header应加后缀\r\n
*      method                  |请求方式
*      additionalData          |随Headers发送的附加的数据
*      dataLength              |additionalData的长度
*      port                    |HTTP服务器的端口
*返回：bool                    |请求成功返回true,反正为false,具体错误查看lastErrorString
*************************************************************************************************************************/
bool HTTPRequest::HTTPOpenRequest(std::string requestURL, std::vector<std::string> additionalHeaders, std::string method, std::vector<char> additionalData, unsigned short port)
{
	std::smatch result;
	std::regex format("http://(.*?)/(.*)");
	std::string host;
	std::string requestDirectory = "/";
	if (std::regex_match(requestURL, result, format))
	{
		host = result[1];
		
		if (result.length() == 3)
		{
			requestDirectory += result[2];
		}
	}
	else
	{
		lastErrorString = "Wrong format for requestURL.";
		return false;
	}

	std::string data(additionalData.begin(), additionalData.end());
	std::string headers = method + " " + requestDirectory + " HTTP/1.1" + "\r\n";
	headers += "Hosts: " + host + "\r\n";
	if (additionalData.empty() == false)
	{	
		headers += "Content-length: ";

		std::stringstream stream;
		std::string temp;
		stream << additionalData.size();
		stream >> temp;
		headers += temp + "\r\n";
	}
	for (auto &element : additionalHeaders)
	{
		headers += element;
	}
	headers += "\r\n";
	headers += data;

	return request(host,port,headers);
}

std::vector<char> HTTPRequest::GetResponseHeaders()
{
	return std::vector<char>();
}

std::vector<char> HTTPRequest::GetResponseMessageBody()
{
	return std::vector<char>();
}


std::string HTTPRequest::GetHeaderFieldValue(std::string fieldName)
{
	return std::string();
}

std::vector<char> HTTPRequest::URLencode(std::vector<char> text)
{
	return std::vector<char>();
}

std::vector<char> HTTPRequest::URLdecode(std::vector<char> text)
{
	return std::vector<char>();
}

std::vector<char> HTTPRequest::UnicodeEscapeToUTF8(std::string text)
{
	return std::vector<char>();
}

std::string HTTPRequest::UTF8ToUnicodeEscape(std::vector<char> text)
{
	return std::string();
}

/************************************************************************************************************************
*向指定的host:port发送数据
*参数： host|目标服务器的域名或IP地址
*       port|目标服务端口
*       data|欲发送的数据
*返回： bool 无错误返回true。 失败返回false，具体错误查看lastErrorString
*************************************************************************************************************************/
bool HTTPRequest::request(std::string host, unsigned short port, std::string & data)
{
	_response.clear();
	int socketFD = socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in server = {};
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	hostent *p = gethostbyname(host.c_str());
	if (p->h_addr_list[0] != nullptr)
	{
		char ip[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, p->h_addr_list[0], ip, INET_ADDRSTRLEN);
		inet_pton(AF_INET, ip, &server.sin_addr.s_addr);
	}
	else
	{
		lastErrorString = "Unable to find ip address for " + host;
		return false;
	}

	if (connect(socketFD, (sockaddr*)&server, sizeof(server)))
	{
		lastErrorString = "connect() error";
		return false;
	}

	if (send(socketFD, data.data(), data.size(), 0) == -1)
	{
		lastErrorString = "send() error";
		return false;
	}

	ssize_t receivedLength = 0;
	char received[65535];
	while ((receivedLength = recv(socketFD, received, 65535, 0)) > 0)
	{
		std::vector<char> buffer(received, received + receivedLength);
		_response.insert(_response.end(), buffer.begin(), buffer.end());
	}
	return true;
}
