#include <vector>
#include <string>
#include <map>

#pragma once

namespace HTTPPacket
{
	enum class ResponseCode : unsigned int {
		OK = 200,
		Created = 201,
		Accepted = 202,
		MovedPermanently = 301,
		Found = 302,
		NotModified = 304,
		BadRequest = 400,
		Unauthorized = 401,
		Forbidden = 403,
		NotFouond = 404,
		MethodNotAllowed = 405,
		InternalServerError = 500,
		NotImplemented = 501,
		BadGateway = 502,
		ServiceUnavailable = 503
	};
	/*!
	HTTP响应报文封装，使组装HTTP响应报文变得容易
	*/
	class HTTPResponsePacket
	{
	public:
		//HTTP协议版本
		std::string version = "HTTP/1.1";
		//HTTP响应状态码，例：200
		ResponseCode code = ResponseCode::OK;
		//响应状态码对应的消息，例：OK
		std::string msg = "OK";
		//响应报文头
		std::multimap<std::string, std::string> responseHeaders;
		//响应报文体
		std::string body;

		/*!
		设定返回状态码和返回消息
		参数：code | 状态码
		*/
		void SetResponseCode(ResponseCode rcode);

		/*!
		添加Set-Cookie报文头，可添加多个
		参数：cookie数值
		*/
		void SetCookie(std::string cookie);

		/*!
		设置Set-Cookie，只会在没有Set-Cookie报文头的情况下才会添加Set-Cookie报文头，存在的情况下会直接向找到的第一个Set-Cookie报文头中添加内容
		使用等号（=）做键与值的分割，使用分号（;）做键值对之间的分割
		参数：key | 欲添加的键
		参数：value | 欲添加的值
		*/
		void SetCookie(std::string key, std::string value);

		/*!
		设定Content-Type
		*/
		void SetContentType(std::string type);

		/*!
		设定Location
		*/
		void SetLocation(std::string location);

		/*!
		设定Connection: Keep-Alive，并设置Keep-Alive为指定的值
		参数：timeout | 超时时间
		参数：maxRequests | 本连接最大的请求数量
		*/
		void SetKeepAlive(unsigned int timeout, unsigned int maxRequests);

		/*!
		设定Server
		*/
		void SetServer(std::string server);

		/*!
		设定Content-Length。本函数默认情况下无需手工调用，在ToString方法中会自动调用
		*/
		void SetContentLength(std::size_t  length);

		/*!
		设定Content-Encoding。
		*/
		void SetContentEncoding(std::string encoding);

		/*!
		将本响应报文对象转换为字符串。会自动根据body的长度添加Content-Length报文头
		*/
		std::string ToString();

	};


	/*!
	HTTP请求报文封装，使解析HTTP请求报文变得容易
	*/
	class HTTPRequestPacket
	{
	public:
		
		//请求方法
		std::string method;
		//请求的整个url
		std::string fullURL;
		//请求路径
		std::string requestPath;
		//请求URL中的参数
		std::string requestParam;
		//HTTP协议版本
		std::string httpVersion;
		//请求头
		std::multimap<std::string, std::string> requestHeaders;
		//请求消息体
		std::string body;

		/*!
		解析requestrParam(?key=value&key=value格式)，转换为键值对关联容器
		返回：解析后的请求参数
		*/
		std::map<std::string, std::string> ParseURLParamter();

		/*!
		检查本报文中所指定报文头是否存在
		参数：header     | 报文头的名称
		参数：ignoreCase | 是否忽略报文头键名的大小写
		返回：报文头的数量，未找到为0
		*/
		unsigned int IsHeaderExist(const std::string& header, bool ignoreCase);

		/*!
		获取本报文中指定报文头的内容
		参数：header | 报文头的名称
		参数：ignoreCase | 是否忽略报文头键名的大小写
		返回：满足报文头名称的内容列表
		*/
		std::vector<std::string> GetHeaderContent(const std::string& header, bool ignoreCase);

		/*!
		获取Cookie中指定项的内容
		参数：key | 项名称
		返回：对应的内容, 如果为空则返回空字符串
		*/
		std::string GetCookieValue(const std::string& key);

		/*!
		获取请求头中X-Real-IP的内容
		参数：ignoreCase | 是否忽略请求头名称的大小写
		*/
		std::string GetRealIP(bool ignoreCase = true);

		/*!
		获取请求头中User-Agent的内容
		参数：ignoreCase | 是否忽略请求头名称的大小写
		*/
		std::string GetUserAgent(bool ignoreCase = true);

		/*!
		获取请求头中Content-Type内容
		参数：ignoreCase | 是否忽略请求头名称的大小写
		*/
		std::string GetContentType(bool ignoreCase = true);

		/*!
		获取请求头中Content-Encoding的内容
		参数：ignoreCase | 是否忽略请求头名称的大小写
		*/
		std::string GetContentEncoding(bool ignoreCase = true);

		/*!
		获取请求头中的Content-Length字段
		参数：ignoreCase | 是否忽略请求头名称的大小写
		*/
		std::string GetContentLength(bool ignoreCase = true);

		/*!
		将本报文类转换为字符串
		*/
		std::string ToString();
	};
}