#include "services.h"

size_t GetValueByKey(std::string &buffer, const std::string &key, const std::string &content)
{
	size_t contentPos = content.find(key);
	if (contentPos == std::string::npos)
	{
		return std::string::npos;
	}
	buffer = content.substr(contentPos + key.length() + 1, content.find("\r\n", contentPos));
	return buffer.length();
}

size_t  LoginService::Login(const std::string & context, std::string &response)
{

	
}

bool LoginService::isVaildPasswordForUser(const std::string & user, const std::string & password)
{
	return false;
}
