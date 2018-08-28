#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "HTTPRequest.h"

#include <regex>
#include <sstream>
#include <algorithm>


/************************************************************************************************************************
*����HTTP���󡣲�֧��HTTPS
*������requestURL              |�����URL��Ӧ�ϸ��ո�ʽhttp://host/directory
*      additionalHeaders       |���ӵ�Headers //ӦΪadditionalData���Content-type��Content-encoding,ÿ��headerӦ�Ӻ�׺\r\n
*      method                  |����ʽ
*      additionalData          |��Headers���͵ĸ��ӵ�����
*      dataLength              |additionalData�ĳ���
*      port                    |HTTP�������Ķ˿�
*���أ�bool                    |����ɹ�����true,����Ϊfalse,�������鿴lastErrorString
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
		
		if (result.size() == 3)
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
	headers += "Host: " + host + "\r\n";
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

/************************************************************************************************************************
*��\r\n\r\n��respouse headers��response body�ķֿ���������respouse headers(��������β��\r\n\r\n)
*��������
*���أ�std::string |�ɹ�����respouse headers��ʧ�ܷ�������response
*************************************************************************************************************************/
std::string HTTPRequest::GetResponseHeaders()
{
	
	std::string str(_response.begin(), _response.end());
	size_t pos = str.find("\r\n\r\n");
	if (pos != str.npos)
	{
		str = str.substr(0, pos);
	}
	
	return str;
}
/************************************************************************************************************************
*��\r\n\r\n��respouse headers��response body�ķֿ���������response body
*��������
*���أ�std::string |�ɹ�����response body��ʧ�ܷ�������response
*************************************************************************************************************************/
std::string HTTPRequest::GetResponseMessageBody()
{
	std::string str(_response.begin(), _response.end());
	size_t pos = str.find("\r\n\r\n");
	if (pos != str.npos)
	{
		str = str.substr(pos + 4, str.size());
	}
	
	return str;
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
*��ָ����host:port��������
*������ host|Ŀ���������������IP��ַ
*       port|Ŀ�����˿�
*       data|�����͵�����
*���أ� bool �޴��󷵻�true�� ʧ�ܷ���false���������鿴lastErrorString
*************************************************************************************************************************/
bool HTTPRequest::request(std::string host, unsigned short port, std::string & data)
{
	_response.clear();
	int socketFD = socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in server = {};
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	hostent *p = gethostbyname(host.c_str());
	if (p != nullptr && p->h_addr_list[0] != nullptr)
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
