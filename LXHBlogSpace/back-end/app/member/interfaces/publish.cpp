#include "publish.h"
#include <tinyHttpd/webstring.h>

#include <sstream>
#include <fstream>

#define SERVER_SIGNATURE "tinyHttpd/0.1a"

void BlogSpacePublish::ModuleInit() noexcept(false)
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
		throw runtime_error("Incomplete mysql configuration. You must provide mysql_user, mysql_auth, and mysql_host");
	}


	//其他的初始化
	InitMysqlConnection();
}

void BlogSpacePublish::InitMysqlConnection() noexcept(false)
{
	try
	{
		mysqlProperty.driverInstance = get_driver_instance();
		mysqlProperty.connection.reset(mysqlProperty.driverInstance->connect(std::string("tcp://") + mysqlProperty.host, mysqlProperty.user, mysqlProperty.password));
	}
	catch (sql::SQLException& e)
	{
		std::stringstream ss;
		ss << "Encountered error while initializing mysql connection.\n";
		ss << "Mysql error code: " << e.getErrorCode();
		ss << "Error Msg: " << e.what();
		throw std::runtime_error(ss.str());
	}
}

void BlogSpacePublish::HTTPPacketHandler(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept
{
	HTTPPacket::HTTPResponsePacket response;
	response.responseHeaders.insert({ "Access-Control-Allow-Origin", "http://blog.leaflxh.com" });

	if (mysqlProperty.connection->isValid() == false)
	{

		if (mysqlProperty.connection->reconnect() == false)
		{
			response.SetResponseCode(HTTPPacket::ResponseCode::ServiceUnavailable);
			connectedClients[clientfd].writeBuffer += response.ToString();

			LogRequestError(clientfd, request, "MySQL connection failed");
			return;
		}
	}

	if (request.requestPath == "/api/v1/content/")
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::Found);
		response.SetLocation("http://blog.leaflxh.com");
		connectedClients[clientfd].writeBuffer += response.ToString();
		LogResponse(clientfd, request, response.code);

		return;
	}

	RaiseHTPPError(clientfd, 404);
	return;
}