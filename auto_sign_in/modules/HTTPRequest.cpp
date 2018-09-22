#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "HTTPRequest.h"

#include <regex>
#include <sstream>
#include <algorithm>
#include <iomanip>


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
bool HTTPRequest::HTTPOpenRequest(std::string requestURL, std::vector<std::string> additionalHeaders, std::string method, std::vector<unsigned char> additionalData, unsigned short port)
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
*以\r\n\r\n将respouse headers与response body的分开，并返回respouse headers(不包含结尾的\r\n\r\n)
*参数：无
*返回：std::string |成功返回respouse headers，失败返回整个response
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
*以\r\n\r\n将respouse headers与response body的分开，并返回response body
*参数：无
*返回：std::string |成功返回response body，失败返回整个response
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

/************************************************************************************************************************
*获取header对应的value(header: value\r\n)，返回所有匹配的结果
*参数：fieldName                |欲获取value的header名
*      headers                  |被搜索的文本
*返回：std::vector<std::string> |无匹配的结果返回空std::vector<std::string>
*************************************************************************************************************************/
std::vector<std::string> HTTPRequest::GetHeaderFieldValue(std::string fieldName, std::string headers)
{
	std::vector<std::string> results;
	std::regex keyValueFormat(fieldName + ": (.*?)\r\n");
	std::regex_iterator <std::string::const_iterator> begin(headers.cbegin(), headers.cend(), keyValueFormat);
	for (auto index = begin; index != std::sregex_iterator(); index++)
	{
		
		results.push_back(index->str(1));
	}
	return results;
}

/************************************************************************************************************************
*根据正则表达式"HTTP/1\..*? (.*?) "获取服务器返回的状态码
*参数：response   | 被搜索的文本
*返回：std::string  | 服务器返回的状态码，匹配失败返回0
*************************************************************************************************************************/
std::string HTTPRequest::GetResponseStatusCode(std::string response)
{
	std::smatch result;
	std::regex format("HTTP/1\..*? (.*?) ");
	if (std::regex_match(response, result, format))
	{
		return "0";
	}
	return result[1];
}

/************************************************************************************************************************
*根据urlencode规则将字节组转为编码后的字符串 例：传入mid=233&csrf=233 输出mid%3d233%26csrf%3d233
*参数：text        | 欲被编码的字节组
*返回：std::string | 编码后的字符串
*************************************************************************************************************************/
std::string HTTPRequest::URLencode(std::vector<unsigned char> text)
{
	std::string convertedBytes;
	std::string temp;
	std::stringstream stream;
	for (auto index : text)
	{

		if (!((index >= 0x41 && index <= 0x5A) || (index >= 0x61 && index <= 0x7A) || (index >= 0x30 && index <= 0x39)) && (index != '-' && index != '_'&& index != '.' && index != '`'))
		{
			stream << std::setfill('0') << std::setw(2)<< std::hex << (static_cast<unsigned int>(index) & 0xFF);
			stream >> temp;
			convertedBytes += '%' + temp;
			stream.clear();
		}
		else
		{
			convertedBytes += index;
		}
	}

	return convertedBytes;
}

/************************************************************************************************************************
*根据URL编码规则将传入的字符串进行解码
*参数：text                                     | 欲被解码的字符串
*返回：std::vector<unsigned char> | 解码后的字节组
*************************************************************************************************************************/
std::vector<unsigned char> HTTPRequest::URLdecode(std::string text)
{
	std::vector<unsigned char> convertedBytes;
	std::stringstream stream;

	std::regex format("%([0-9a-fA-F]{2})");
	
	for (auto index = text.cbegin(); index != text.cend(); )
	{
		if (*index ==  '%')
		{
			std::string assumedEncodedString(index, index + 3);
			std::smatch result;
			if (std::regex_match(assumedEncodedString, result, format))
			{
				unsigned int byteValue;
				stream << std::hex << result[1];
				stream >> byteValue;
				stream.clear();

				convertedBytes.push_back(byteValue);
				index += 3;
			}
			else
			{
				convertedBytes.push_back(*index);
				index++;
			}
		}
		else
		{
			convertedBytes.push_back(*index);
			index++;
		}
	}
	
	return convertedBytes;
}

/************************************************************************************************************************
*将Unicode-escaped字符串转为UTF8字节,输入 \u4E2D\u6587 返回 E4 B8 AD E6 96 87
*参数：text                       |Unicode-escaped字符串
*返回：std::vector<unsigned char> |UTF8字节
*************************************************************************************************************************/
std::vector<unsigned char> HTTPRequest::UnicodeEscapeToUTF8(std::string text)
{
	std::vector<unsigned char> convertedStr;
	std::regex format16("\\\\u([0-9a-fA-F]{4})");
	std::regex format32("\\\\U([0-9a-fA-F]{8})");
	for (auto index = text.begin(); index != text.end();)
	{
		if (*index == '\\')
		{
			size_t step = 0;
			if (text.end() - index >= 10)
			{
				step = 10;
			}
			else if (text.end() - index < 10 && text.end() - index >= 6)
			{
				step = 6;
			}
			else
			{
				convertedStr.push_back(*index);
				index++;
				continue;
			}

			std::smatch result;
			std::string assumedUnicodeEscape(index, index + step);
			if (std::regex_search(assumedUnicodeEscape, result, format16) || std::regex_search(assumedUnicodeEscape, result, format32))
			{
				index += result[1].length() + 2;
				unsigned int value = 0;
				std::stringstream ss;
				ss << std::hex << result[1];
				ss >> value;

				if (0 <= value && value <= 0x7F)
				{
					convertedStr.push_back(value);
				}
				else if (0x80 <= value && value <= 0x7FF)
				{
					unsigned int first = (value & 0x7C0) >> 5;
					unsigned int second = value & 0x3F;

					first += 0xC0;
					second += 0x80;

					convertedStr.push_back(first);
					convertedStr.push_back(second);
				}
				else if (0x800 <= value && value <= 0xD7FF || 0xE000 <= value && value <= 0xFFFF)
				{
					unsigned int first = (value & 0xF000) >> 12;
					unsigned int second = (value & 0xFC0) >> 6;
					unsigned int third = value & 0x3F;

					first += 0xE0;
					second += 0x80;
					third += 0x80;

					convertedStr.push_back(first);
					convertedStr.push_back(second);
					convertedStr.push_back(third);
				}
				else if (0x10000 <= value && value <= 0x10FFFF)
				{
					unsigned int first = (value & 0x1C0000) >> 18;
					unsigned int second = (value & 0x3F000) >> 12;
					unsigned int third = (value & 0xFC0) >> 6;
					unsigned int fourth = value & 0x3F;

					first += 0xF0;
					second += 0x80;
					third += 0x80;
					fourth += 0x80;

					convertedStr.push_back(first);
					convertedStr.push_back(second);
					convertedStr.push_back(third);
					convertedStr.push_back(fourth);
				}
				else
				{
					convertedStr.push_back(*index);
					index++;
				}
			}
			else
			{
				convertedStr.push_back(*index);
				index++;
			}
		}
		else
		{
			convertedStr.push_back(*index);
			index++;
		}
	}

	return convertedStr;
}

std::string HTTPRequest::UTF8ToUnicodeEscape(std::vector<unsigned char> text)
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
		std::vector<unsigned char> buffer(received, received + receivedLength);
		_response.insert(_response.end(), buffer.begin(), buffer.end());
	}
	return true;
}
