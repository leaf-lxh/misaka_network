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
	this->lastErrorCode = "";
	this->lastErrorMsg = "";
	//邮箱名允许数字、字母、英文句点、加号、减号，下划线，邮件提供商允许QQ邮箱，网易126、163邮箱、谷歌gamil邮箱、foxmail邮箱、新浪邮箱
	std::regex format("[0-9a-zA-Z\.\+\-_]+?@[0-9a-zA-Z\.]*?(qq\.com|163\.com|gmail\.com|foxmail\.com|126\.com|sina\.com)");
	if (!std::regex_match(email, format))
	{
		this->lastErrorCode = "10000";
		return "";
	}

	//生成验证码，并存储至lxhpassnote_user->signup_auth
	unsigned long seed = time(nullptr);
	for (int i = 0; i < email.length() % 10; ++i)
	{
		seed += email[i] + i;
	}
	srand(seed);

	std::string code;
	std::stringstream stream;
	stream << std::setw(6) << std::setfill('0') << (rand() % 999999);
	stream >> code;

	try
	{
		sql::Driver *driver = get_driver_instance();
		sql::Connection *connection = driver->connect(this->mysqlHost, this->mysqlUser, this->mysqlPassword);
		connection->setSchema("lxhpassnote_user");
		sql::PreparedStatement *sqlStatement = connection->prepareStatement("insert into signup_auth values(?,?,?,?)");

		sqlStatement->setString(1, email);
		sqlStatement->setString(2, "");
		sqlStatement->setString(3, code);
		sqlStatement->setInt64(4, time(nullptr));

		if (sqlStatement->executeUpdate() != 1)
		{
			this->lastErrorCode = "10001";
			return "";
		}
	}
	catch (sql::SQLException &e)
	{
		stream.clear();
		stream.str("");
		stream << e.getErrorCode();
		this->lastErrorMsg = stream.str();
		this->lastErrorCode = "10002";
		return "";
	}


	SSLEmailService::EmailInfo context;
	context.sender = "LXHPassNote";
	context.senderEmail = emailAccount;
	context.password = emailPassword;
	context.smtpServer = smtpserver;
	context.serverPort = smtpport;
	context.charset = "utf-8";
	context.recipient = "注册者";
	context.recipientEmail = email;

	context.subject = "罗小黑的密码笔记本--注册验证码";
	context.MIMEContentType = "text/html";
	context.message = "注册验证码：";


	stream.clear();
	stream.str("");
	stream << "<span style=\"font-weight:800;color:#3140b9\">" << code << "</span>" << ", 5分钟内有效";
	context.message += stream.str();

	SSLEmailService poster;
	int retcode = poster.SendEmail(context, true);
	if (retcode != 0)
	{
		this->lastErrorCode = "10003";
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
