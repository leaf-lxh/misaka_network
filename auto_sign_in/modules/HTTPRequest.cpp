#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "HTTPRequest.h"

std::string HTTPRequest::HTTPOpenRequest(std::string requestURL, std::vector<std::string> additionalHeaders, std::string method, char * additionalData, unsigned long long dataLength)
{

	return std::string();
}

std::vector<std::string> HTTPRequest::SplitResponse(std::string response)
{
	return std::vector<std::string>();
}

std::string HTTPRequest::GetHeaderFieldValue(std::string fieldName)
{
	return std::string();
}

std::string HTTPRequest::URLencode(std::string text)
{
	return std::string();
}

std::string HTTPRequest::URLdecode(std::string text)
{
	return std::string();
}

std::string HTTPRequest::UnicodeEscapeToUTF8(std::string text)
{
	return std::string();
}

std::string HTTPRequest::UTF8ToUnicodeEscape(std::string text)
{
	return std::string();
}
