#pragma once
#include <string>

size_t GetValueByKey(std::string &buffer, const std::string &key, const std::string &content);

class LoginService
{
public:
	size_t Login(const std::string & context, std::string &response);

private:
	bool isVaildPasswordForUser(const std::string &user, const std::string &password);
};