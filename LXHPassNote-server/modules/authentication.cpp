#include "authentication.h"

#include "email.h"

#include <sstream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <regex>
#include <iomanip>

#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>

std::string Authentication::ToEmailAuthCode(std::string email, std::string smtpserver, std::string smtpport, std::string emailAccount, std::string emailPassword)
{
	lastErrorMsg = "";

	SSLEmailService::EmailInfo context;
	context.sender = u8"LXHPassNote";
	context.senderEmail = emailAccount;
	context.password = emailPassword;
	context.smtpServer = smtpserver;
	context.serverPort = smtpport;
	context.charset = "utf-8";
	context.recipient = u8"注册者";
	context.recipientEmail = email;

	context.subject = "罗小黑的密码笔记本--注册验证码";
	context.MIMEContentType = "text/html";
	context.message = "注册验证码：";

	unsigned long seed = time(nullptr);
	for (int i = 0; i < email.length() % 10; ++i)
	{
		seed += email[i] + i;
	}
	srand(seed);

	std::string code;
	std::stringstream stream;
	stream << std::setw(6) << std::setfill('0') <<(rand() % 999999);
	stream >> code ;

	stream.clear();
	stream.str("");

	stream << "<span style=\"font-weight:800;color:#3140b9\">" << code << "</span>" << ", 5分钟内有效";
	context.message += stream.str();

	SSLEmailService poster;
	if (poster.SendEmail(context, true))
	{
		this->lastErrorMsg = poster.lastErrorMsg;
		return "";
	}
	
	return code;
}

std::string Authentication::SignUp(std::string password, std::string email, std::string authcode, std::string phone, std::string username)
{
	return std::string();
}

std::string Authentication::SignIn(std::string username, std::string password)
{
	return std::string();
}

bool Authentication::UpdateUserDetails(std::string id, std::string token, std::string username, std::string avatar, std::string phone)
{
	return false;
}

bool Authentication::UpdateUserPassword(std::string id, std::string authcode, std::string newpassword)
{
	return false;
}

std::string Authentication::GetIDByEmail(std::string email)
{
	return std::string();
}
