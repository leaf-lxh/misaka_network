#include "HTTPPacket.h"
#include "webstring.h"

#include <sstream>

std::map<std::string, std::string> HTTPReuqestPacket::ParseURLParamter()
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


unsigned int HTTPReuqestPacket::IsHeaderExist(const std::string &header, bool ignoreCase)
{
	unsigned int count = 0;
	for (auto kv : headers)
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

std::vector<std::string> HTTPReuqestPacket::GetHeaderContent(const std::string &header, bool ignoreCase)
{
	std::vector<std::string> values;
	for (auto kv : headers)
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

std::string HTTPReuqestPacket::GetCookieValue(const std::string& key)
{
	auto cookieset = GetHeaderContent("cookie", false);
	if (cookieset.size() == 0)
	{
		return "";
	}
	return webstring::ParseKeyValue(cookieset[0], '=', ';')[key];
}

std::string HTTPReuqestPacket::GetRealIP(bool ignoreCase = true)
{
	auto headerset = GetHeaderContent("X-Real-IP", ignoreCase);
	if (headerset.size() == 0)
	{
		return "";
	}
	
	return headerset[0];
}

std::string HTTPReuqestPacket::GetUserAgent(bool ignoreCase = true)
{
	auto headerset = GetHeaderContent("User-Agent", ignoreCase);
	if (headerset.size() == 0)
	{
		return "";
	}

	return headerset[0];
}

std::string HTTPReuqestPacket::GetContentType(bool ignoreCase = true)
{
	auto headerset = GetHeaderContent("Content-Type", ignoreCase);
	if (headerset.size() == 0)
	{
		return "";
	}

	return headerset[0];
}

std::string HTTPReuqestPacket::GetContentEncoding(bool ignoreCase = true)
{
	auto headerset = GetHeaderContent("Content-Encoding", ignoreCase);
	if (headerset.size() == 0)
	{
		return "";
	}

	return headerset[0];
}

std::string HTTPReuqestPacket::ToString()
{
	std::stringstream stream;
	stream << method << " " << fullURL << " " << httpVersion << "\r\n";
	for (auto pair : headers)
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
	case HTTPResponsePacket::ResponseCode::OK:	
		msg = "OK";
		break;
	case HTTPResponsePacket::ResponseCode::Created:
		msg = "Created";
		break;
	case HTTPResponsePacket::ResponseCode::Accepted:
		msg = "Accepted";
		break;
	case HTTPResponsePacket::ResponseCode::MovedPermanently:
		msg = "Moved Permanently";
		break;
	case HTTPResponsePacket::ResponseCode::Found:
		msg = "Found";
		break;
	case HTTPResponsePacket::ResponseCode::NotModified:
		msg = "Not Modified";
		break;
	case HTTPResponsePacket::ResponseCode::BadRequest:
		msg = "Bad Request";
		break;
	case HTTPResponsePacket::ResponseCode::Unauthorized:
		msg = "Unauthorized";
		break;
	case HTTPResponsePacket::ResponseCode::Forbidden:
		msg = "Forbidden";
		break;
	case HTTPResponsePacket::ResponseCode::NotFouond:
		msg = "Not Fouond";
		break;
	case HTTPResponsePacket::ResponseCode::MethodNotAllowed:
		msg = "Method Not Allowed";
		break;
	case HTTPResponsePacket::ResponseCode::InternalServerError:
		msg = "Internal Server Error";
		break;
	case HTTPResponsePacket::ResponseCode::NotImplemented:
		msg = "Not Implemented";
		break;
	case HTTPResponsePacket::ResponseCode::BadGateway:
		msg = "Bad Gateway";
		break;
	case HTTPResponsePacket::ResponseCode::ServiceUnavailable:
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