#include "passport.h"
#include <sstream>
#include <iostream>

#include <tinyHttpd/webstring.h>

#include <mysql_connection.h>

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#define SERVER_SIGNATURE "tinyHttpd/0.1a"

void BlogSpacePassport::ModuleInit() noexcept(false)
{
	using namespace std;
	//初始化配置读取
	map<string, string> setting = Init();

	//MYSQL数据库信息读取
	if (setting.count("mysql_user") && setting.count("mysql_auth") && setting.count("mysql_host"))
	{
		mysqlProperty.user = webstring::strip(setting["mysql_user"], "\"\' ");
		mysqlProperty.password = webstring::strip(setting["mysql_auth"], "\"\' ");
		mysqlProperty.host = webstring::strip(setting["mysql_host"], "\"\' ");
	}
	else
	{
		throw runtime_error("Incomplete mysql configuration. You must provide mysql_user, mysql_auth, mysql_host");
	}



	//初始化路由表
	InitRouteTabel();
}

void BlogSpacePassport::InitRouteTabel() noexcept
{
	AddRoutePath("/blog/v1/passport/");
	AddRoutePath("/blog/v1/passport/login");
	AddRoutePath("/blog/v1/passport/register");
	AddRoutePath("/blog/v1/passport/GetUserInfo");
}

void BlogSpacePassport::InitMysqlConnection() noexcept(false)
{
	try
	{
		mysqlProperty.driverInstance = get_driver_instance();
		mysqlProperty.connection.reset(mysqlProperty.driverInstance->connect(std::string("tcp://") + mysqlProperty.host, mysqlProperty.user, mysqlProperty.password));
		mysqlProperty.connection->setSchema("LXHBlogSpace");
	}
	catch (sql::SQLException &e)
	{
		std::stringstream ss;
		ss << std::skipws;
		ss << "Encountered error while initializing mysql connection.\n";
		ss << "Mysql error code: " << e.getErrorCode();
		throw std::runtime_error(ss.str());
	}
}

void BlogSpacePassport::HTTPPacketHandler(int clientfd, TinyHttpd::HTTPRequestPacket request) noexcept
{
	std::cout << request.requestPath << std::endl;

	if (serverProperty.routeTable.count(request.requestPath) == 0)
	{
		RaiseHTPPError(clientfd, 404);
		return;
	}

	LogRequestError(clientfd, request, "received a request.");

	TinyHttpd::HTTPResponsePacket response;
	if (request.requestPath == "/blog/v1/passport/GetUserInfo")
	{
		try
		{
			response = GetUserInfo(clientfd, request);
			connectedClients[clientfd].writeBuffer += ResponseToString(response);
		}
		catch (std::runtime_error e)
		{
			RaiseHTPPError(clientfd, 500);
		}
		return;
	}

	if (request.requestPath == "/blog/v1/passport/login")
	{
		try
		{
			response = Login(clientfd, request);
			connectedClients[clientfd].writeBuffer += ResponseToString(response);
		}
		catch (std::runtime_error e)
		{
			LogRequestError(clientfd, request, e.what());
		}
		return;
	}

	if (request.requestPath == "/blog/v1/passport/register")
	{
		try
		{
			response = Register(clientfd, request);
			connectedClients[clientfd].writeBuffer += ResponseToString(response);
		}
		catch (std::runtime_error e)
		{
			RaiseHTPPError(clientfd, 500);
		}
		return;
	}
	
	RaiseHTPPError(clientfd, 501);
}


TinyHttpd::HTTPResponsePacket BlogSpacePassport::Login(int clientfd, TinyHttpd::HTTPRequestPacket request) noexcept(false)
{
	HTTPResponsePacket response;
	if (request.method != "POST")
	{
		RaiseHTPPError(clientfd, 405);
		throw std::runtime_error("requested with unspported method");
	}
	return response;
}

TinyHttpd::HTTPResponsePacket BlogSpacePassport::Register(int clientfd, TinyHttpd::HTTPRequestPacket request) noexcept(false)
{
	HTTPResponsePacket response;
	return response;
}

TinyHttpd::HTTPResponsePacket BlogSpacePassport::GetUserInfo(int clientfd, TinyHttpd::HTTPRequestPacket request) noexcept(false)
{
	HTTPResponsePacket response;
	response.code = "200 OK";
	response.version = "HTTP/1.1";
	response.responseHeaders.insert({ "Server", SERVER_SIGNATURE });
	std::stringstream param;
	param << std::noskipws;
	param << "timeout=" << serverProperty.timeout << ", max=" << serverProperty.maxRequestsNum;
	response.responseHeaders.insert({ "Connection", "Keep-Alive" });
	response.responseHeaders.insert({ "Keep-Alive", param.str() });


	response.body = "{\"vaild\": true, \"name\": \"LegendLXH\", \"avatar\": \"https://blog.leaflxh.com/lxhcat.jpg\"}";
	response.responseHeaders.insert({ "Content-Encoding", "plain" });
	response.responseHeaders.insert({ "Content-Length", std::to_string(response.body.length()) });
	response.responseHeaders.insert({ "Content-Type", "text/plain; charset=utf8" });
	return response;
}