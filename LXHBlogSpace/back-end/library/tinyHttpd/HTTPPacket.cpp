#include "HTTPPacket.h"
#include "webstring.h"

#include <sstream>

namespace HTTPPacket
{
	std::map<std::string, std::string> HTTPRequestPacket::ParseURLParamter()
	{
		using namespace std;
		size_t paramStart = requestParam.find("?");
		if (paramStart == string::npos)
		{
			return {};
		}

		std::string param = requestParam.substr(paramStart + 1);
		return webstring::ParseKeyValue(param);
	}


	unsigned int HTTPRequestPacket::IsHeaderExist(const std::string& header, bool ignoreCase)
	{
		unsigned int count = 0;
		for (auto kv : requestHeaders)
		{
			if (ignoreCase == true)
			{
				if (webstring::tolower(kv.first) == webstring::tolower(header))
				{
					++count;
				}
			}
			else
			{
				if (kv.first == header)
				{
					++count;
				}
			}
		}

		return count;
	}

	std::vector<std::string> HTTPRequestPacket::GetHeaderContent(const std::string& header, bool ignoreCase)
	{
		std::vector<std::string> values;
		for (auto kv : requestHeaders)
		{
			if (ignoreCase == true)
			{
				if (webstring::tolower(kv.first) == webstring::tolower(header))
				{
					values.push_back(kv.second);
				}
			}
			else
			{
				if (kv.first == header)
				{
					values.push_back(kv.second);
				}
			}
		}

		return values;
	}

	std::string HTTPRequestPacket::GetCookieValue(const std::string& key)
	{
		auto cookieset = GetHeaderContent("cookie", false);
		if (cookieset.size() == 0)
		{
			return "";
		}
		return webstring::ParseKeyValue(cookieset[0], '=', ';')[key];
	}

	std::string HTTPRequestPacket::GetRealIP(bool ignoreCase)
	{
		auto headerset = GetHeaderContent("X-Real-IP", ignoreCase);
		if (headerset.size() == 0)
		{
			return "";
		}

		return headerset[0];
	}

	std::string HTTPRequestPacket::GetUserAgent(bool ignoreCase)
	{
		auto headerset = GetHeaderContent("User-Agent", ignoreCase);
		if (headerset.size() == 0)
		{
			return "";
		}

		return headerset[0];
	}

	std::string HTTPRequestPacket::GetContentType(bool ignoreCase)
	{
		auto headerset = GetHeaderContent("Content-Type", ignoreCase);
		if (headerset.size() == 0)
		{
			return "";
		}

		return headerset[0];
	}

	std::string HTTPRequestPacket::GetContentEncoding(bool ignoreCase)
	{
		auto headerset = GetHeaderContent("Content-Encoding", ignoreCase);
		if (headerset.size() == 0)
		{
			return "";
		}

		return headerset[0];
	}

	std::string HTTPRequestPacket::GetContentLength(bool ignoreCase)
	{
		auto headerset = GetHeaderContent("Content-Length", ignoreCase);
		if (headerset.size() == 0)
		{
			return "";
		}

		return headerset[0];
	}

	std::string HTTPRequestPacket::ToString()
	{
		std::stringstream stream;
		stream << method << " " << fullURL << " " << httpVersion << "\r\n";
		for (auto pair : requestHeaders)
		{
			stream << pair.first << ": " << pair.second << "\r\n";
		}
		stream << "\r\n" << body;

		return stream.str();
	}

	void HTTPResponsePacket::SetResponseCode(ResponseCode rcode)
	{
		code = rcode;
		switch (rcode)
		{
		case ResponseCode::OK:
			msg = "OK";
			break;
		case ResponseCode::Created:
			msg = "Created";
			break;
		case ResponseCode::Accepted:
			msg = "Accepted";
			break;
		case ResponseCode::MovedPermanently:
			msg = "Moved Permanently";
			break;
		case ResponseCode::Found:
			msg = "Found";
			break;
		case ResponseCode::NotModified:
			msg = "Not Modified";
			break;
		case ResponseCode::BadRequest:
			msg = "Bad Request";
			break;
		case ResponseCode::Unauthorized:
			msg = "Unauthorized";
			break;
		case ResponseCode::Forbidden:
			msg = "Forbidden";
			break;
		case ResponseCode::NotFouond:
			msg = "Not Fouond";
			break;
		case ResponseCode::MethodNotAllowed:
			msg = "Method Not Allowed";
			break;
		case ResponseCode::InternalServerError:
			msg = "Internal Server Error";
			break;
		case ResponseCode::NotImplemented:
			msg = "Not Implemented";
			break;
		case ResponseCode::BadGateway:
			msg = "Bad Gateway";
			break;
		case ResponseCode::ServiceUnavailable:
			msg = "Service Unavailable";
			break;
		default:
			break;
		}
	}

	void HTTPResponsePacket::SetCookie(std::string cookie)
	{
		responseHeaders.insert({ "Set-Cookie", cookie });
	}

	void HTTPResponsePacket::SetCookie(std::string key, std::string value)
	{
		auto setcookieIter = responseHeaders.find("Set-Cookie");
		if (setcookieIter == responseHeaders.end())
		{
			SetCookie(key + "=" + value);
		}
		else
		{
			setcookieIter->second = setcookieIter->second + "; " + key + "=" + value;
		}
	}

	void HTTPResponsePacket::SetContentType(std::string type)
	{
		responseHeaders.insert({ "Content-Type", type });
	}

	void HTTPResponsePacket::SetLocation(std::string location)
	{
		responseHeaders.insert({ "Location", location });
	}

	void HTTPResponsePacket::SetKeepAlive(unsigned int timeout, unsigned int maxRequests)
	{
		responseHeaders.insert({ "Connection", "Keep-Alive" });
		responseHeaders.insert({ "Keep-Alive", std::string("timeout=") + std::to_string(timeout) + ", max=" + std::to_string(maxRequests) });
	}

	void HTTPResponsePacket::SetServer(std::string server)
	{
		responseHeaders.insert({ "Server", server });
	}

	void HTTPResponsePacket::SetContentLength(unsigned int length)
	{
		responseHeaders.insert({ "Content-Length", std::to_string(length) });
	}

	void HTTPResponsePacket::SetContentEncoding(std::string encoding)
	{
		responseHeaders.insert({ "Content-Encoding", encoding });
	}

	std::string HTTPResponsePacket::ToString()
	{
		SetContentLength(body.length());

		std::stringstream stream;
		stream << version << " " << static_cast<unsigned int>(code) << " " << msg << "\r\n";
		for (auto header : responseHeaders)
		{
			stream << header.first << ": " << header.second << "\r\n";
		}

		stream << "\r\n" << body;
		return stream.str();
	}
}