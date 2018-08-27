#include <string>
#include <vector>


#pragma once

class HTTPRequest {
public:
	HTTPRequest() = default;
	~HTTPRequest() = default;

	std::string HTTPOpenRequest(std::string requestURL, std::vector<std::string> additionalHeaders, std::string method, char *additionalData, unsigned long long dataLength);

	std::vector<std::string> SplitResponse(std::string response);

	std::string GetHeaderFieldValue(std::string fieldName);

	std::string URLencode(std::string text);

	std::string URLdecode(std::string text);

	std::string UnicodeEscapeToUTF8(std::string text);

	std::string UTF8ToUnicodeEscape(std::string text);
	
};