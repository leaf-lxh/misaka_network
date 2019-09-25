#include "passport.h"
#include <sstream>
#include <iostream>

#include <tinyHttpd/webstring.h>

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
	InitMysqlConnection();
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
	}
	catch (sql::SQLException &e)
	{
		std::stringstream ss;
		ss << std::skipws;
		ss << "Encountered error while initializing mysql connection.\n";
		ss << "Mysql error code: " << e.getErrorCode(); 
		ss << "Error Msg: " << e.what();
		throw std::runtime_error(ss.str());
	}
}

void BlogSpacePassport::HTTPPacketHandler(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept
{
	std::cout << request.requestPath << std::endl;

	if (serverProperty.routeTable.count(request.requestPath) == 0)
	{
		RaiseHTPPError(clientfd, 404);
		return;
	}

	LogRequestError(clientfd, request, "received a request.");

	HTTPPacket::HTTPResponsePacket response;
	if (request.requestPath == "/blog/v1/passport/GetUserInfo")
	{
		try
		{
			response = GetUserInfo(clientfd, request);
			connectedClients[clientfd].writeBuffer += response.ToString();
		}
		catch (std::runtime_error e)
		{
			LogRequestError(clientfd, request, e.what());
		}
		return;
	}

	if (request.requestPath == "/blog/v1/passport/login")
	{
		try
		{
			response = Login(clientfd, request);
			connectedClients[clientfd].writeBuffer += response.ToString();
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
			connectedClients[clientfd].writeBuffer += response.ToString();
		}
		catch (std::runtime_error e)
		{
			LogRequestError(clientfd, request, e.what());
		}
		return;
	}
	
	RaiseHTPPError(clientfd, 501);
}


HTTPPacket::HTTPResponsePacket BlogSpacePassport::Login(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false)
{
	using boost::property_tree::ptree;
	using boost::property_tree::write_json;

	HTTPPacket::HTTPResponsePacket response;
	response.SetServer(SERVER_SIGNATURE);

	if (request.method != "POST")
	{
		RaiseHTPPError(clientfd, 405);
		throw std::runtime_error("requested with unspported method");
	}

	std::map<std::string, std::string> requestParam = webstring::ParseKeyValue(request.body);
	std::string username = requestParam["username"];
	std::string passwordHex = requestParam["phex"];

	if ((username == "") || (passwordHex == ""))
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::OK);

		ptree jsonTree;
		jsonTree.put("ecode", -2);
		jsonTree.put("reason", "用户名或密码错误");
		std::ostringstream buffer;
		write_json(buffer, jsonTree, false);

		response.body = buffer.str();
	}
	else
	{

		if (request.GetContentType() == "application/x-www-form-urlencoded")
		{
			username = webstring::URLdecode(username);
			passwordHex = webstring::URLdecode(passwordHex);
		}

		try
		{
			//首先根据用户名或邮箱来获取用户UUID
			std::string userUUID;

			mysqlProperty.connection->setSchema("lxhblogspace_passport");
			PtrPreparedStatement statement(mysqlProperty.connection->prepareStatement("SELECT user_uuid FROM user_info WHERE username = ? OR email = ?"));
			
			statement->setString(1, username);
			statement->setString(2, username);
			PtrResultSet queryResult(statement->executeQuery());
			while (queryResult->next())
			{
				userUUID = queryResult->getString(1);
				break;
			}

			
			if (userUUID == "")
			{
				//未找到此用户
				response.SetResponseCode(HTTPPacket::ResponseCode::OK);

				ptree jsonTree;
				jsonTree.put("ecode", -2);
				jsonTree.put("reason", "用户名或密码错误");
				std::ostringstream buffer;
				write_json(buffer, jsonTree, false);

				response.body = buffer.str();
			}
			else
			{
				std::string authHex;
				std::string authSalt;

				PtrPreparedStatement statement(mysqlProperty.connection->prepareStatement("SELECT auth_hex, auth_salt FROM user_auth WHERE user_uuid = ?"));

				statement->setString(1, userUUID);
				PtrResultSet queryResult(statement->executeQuery());
				while (queryResult->next())
				{
					authHex = queryResult->getString(1);
					authSalt = queryResult->getString(2);
					break;
				}

				if (authHex == webstring::sha1(passwordHex + authSalt))
				{
					std::string token = webstring::sha1(authHex + webstring::GenUUID()) + "AAAAAAAAAAAAAA";
					token += webstring::sha1(token);

					PtrPreparedStatement statement(mysqlProperty.connection->prepareStatement("INSERT INTO user_token VALUES(?,?,?)"));
					statement->setString(1, userUUID);
					statement->setString(2, token);
					statement->setString(3, webstring::GenTimeStamp());
					statement->executeUpdate();

					response.SetCookie("_sessionToken",token);
					response.SetResponseCode(HTTPPacket::ResponseCode::Found);
				}
				else
				{
					response.SetResponseCode(HTTPPacket::ResponseCode::OK);

					ptree jsonTree;
					jsonTree.put("ecode", -2);
					jsonTree.put("reason", "用户名或密码错误");
					std::ostringstream buffer;
					write_json(buffer, jsonTree, false);

					response.body = buffer.str();
				}
			}
		}
		catch (sql::SQLException e)
		{
			response.SetResponseCode(HTTPPacket::ResponseCode::ServiceUnavailable);
			if (serverProperty.verbose >= VerboseLevel::essential)
			{
				LogRequestError(clientfd, request, std::string("BlogSpacePassport::Login(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()) + ", msg: " + e.what());
			}
			else
			{
				LogRequestError(clientfd, request, std::string("BlogSpacePassport::Login(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()));
			}
		}
	}
	return response;
}

HTTPPacket::HTTPResponsePacket BlogSpacePassport::Register(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false)
{
	HTTPPacket::HTTPResponsePacket response;
	return response;
}

HTTPPacket::HTTPResponsePacket BlogSpacePassport::GetUserInfo(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false)
{
	HTTPPacket::HTTPResponsePacket response;
	response.SetResponseCode(HTTPPacket::ResponseCode::OK);
	response.SetServer(SERVER_SIGNATURE);

	response.SetKeepAlive(serverProperty.timeout, serverProperty.maxRequestsNum);


	response.body = "{\"vaild\": true, \"name\": \"LegendLXH\", \"avatar\": \"https://blog.leaflxh.com/lxhcat.jpg\"}";
	response.SetContentType("text/plain; charset=utf8");
	response.SetContentEncoding("plain");

	return response;
}