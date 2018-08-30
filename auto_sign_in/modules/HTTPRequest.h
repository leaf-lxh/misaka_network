#include <string>
#include <vector>


#pragma once

class HTTPRequest {
public:
	HTTPRequest() = default;
	~HTTPRequest() = default;

	std::string lastErrorString;
	
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
	bool HTTPOpenRequest(std::string requestURL, std::vector<std::string> additionalHeaders, std::string method, std::vector<char> additionalData, unsigned short port);

	/************************************************************************************************************************
	*以\r\n\r\n将respouse headers与response body的分开，并返回respouse headers(不包含结尾的\r\n\r\n)
	*参数：无
	*返回：std::string |成功返回respouse headers，失败返回整个response
	*************************************************************************************************************************/
	std::string GetResponseHeaders();

	/************************************************************************************************************************
	*以\r\n\r\n将respouse headers与response body的分开，并返回response body
	*参数：无
	*返回：std::string |成功返回response body，失败返回整个response
	*************************************************************************************************************************/
	std::string GetResponseMessageBody();

	/************************************************************************************************************************
	*获取header对应的value(header: value\r\n)，返回所有匹配的结果
	*参数：fieldName                |欲获取value的header名
	*      headers                  |被搜索的文本
	*返回：std::vector<std::string> |无匹配的结果返回空std::vector<std::string>
	*************************************************************************************************************************/
	std::vector<std::string> GetHeaderFieldValue(std::string fieldName, std::string headers);

	std::vector<char> URLencode(std::vector<char>);

	std::vector<char> URLdecode(std::vector<char>);

	std::vector<char> UnicodeEscapeToUTF8(std::string text);

	std::string UTF8ToUnicodeEscape(std::vector<char>);
	
private:
	std::vector<char> _response;

	/************************************************************************************************************************
	*向指定的host:port发送数据
	*参数： host|目标服务器的域名或IP地址
	*       port|目标服务端口
	*       data|欲发送的数据
	*返回： bool 无错误返回true。 失败返回false，具体错误查看lastErrorString
	*************************************************************************************************************************/
	bool request(std::string host,unsigned short port,std::string &data);
};