#include <string>
#include <vector>


#pragma once

class HTTPRequest {
public:
	HTTPRequest() = default;
	~HTTPRequest() = default;

	std::string lastErrorString;
	


	/************************************************************************************************************************
	*进行HTTP请求，不支持HTTPS
	*参数：requestURL              |请求的URL，应严格按照格式http://host/directory
	*      additionalHeaders       |附加的Headers
	*      method                  |请求方式
	*      additionalData          |随Headers发送的附加的数据
	*      dataLength              |additionalData的长度
	*      port                    |HTTP服务器的端口
	*返回：string                  |本次请求后服务器返回的所有数据
	*************************************************************************************************************************/
	bool HTTPOpenRequest(std::string requestURL, std::vector<std::string> additionalHeaders, std::string method, std::vector<char> additionalData, unsigned short port);

	std::vector<char> GetResponseHeaders();

	std::vector<char> GetResponseMessageBody();

	std::string GetHeaderFieldValue(std::string fieldName);

	std::vector<char> URLencode(std::vector<char>);

	std::vector<char> URLdecode(std::vector<char>);

	std::vector<char> UnicodeEscapeToUTF8(std::string text);

	std::string UTF8ToUnicodeEscape(std::vector<char>);
	
private:
	std::vector<char> _response;
	bool request(std::string host,unsigned short port,std::string &data);
};