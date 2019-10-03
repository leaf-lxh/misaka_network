#include "passport.h"
#include <sstream>
#include <iostream>
#include <regex>

#include <tinyHttpd/webstring.h>
#include <sslemail/email.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <pthread.h>

#define SERVER_SIGNATURE "tinyHttpd/0.1a"


//字符串应当用string.data()赋值，以避免COW产生的问题
void *AsyncSendEmail(void *email)
{
	SSLEmailService* service = (SSLEmailService*)email;
	service->SendEmail(true);
	delete service;
	pthread_exit(0);
}


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
		throw runtime_error("Incomplete mysql configuration. You must provide mysql_user, mysql_auth, and mysql_host");
	}

	//SSL邮件服务配置读取
	if (setting.count("email_server") && setting.count("email_port") && setting.count("email_account") && setting.count("email_auth"))
	{
		emailProperty.server = webstring::strip(setting["email_server"], "\'\"");
		emailProperty.port = setting["email_port"];
		emailProperty.sender = webstring::strip(setting["email_account"], "\'\"");
		emailProperty.auth = webstring::strip(setting["email_auth"], "\'\"");
	}
	else
	{
		throw runtime_error("Incomplete email configuration. You must provide email_server, email_port, email_account, and email_auth");
	}


	//其他的初始化
	InitRouteTabel();
	InitMysqlConnection();
}

void BlogSpacePassport::InitRouteTabel() noexcept
{
	AddRoutePath("/api/v1/passport/");
	AddRoutePath("/api/v1/passport/login");
	AddRoutePath("/api/v1/passport/register");
	AddRoutePath("/api/v1/passport/GetUserInfo");
	AddRoutePath("/api/v1/passport/CheckUserExist");
	AddRoutePath("/api/v1/passport/CheckEmailExist");
	AddRoutePath("/api/v1/passport/SendEmailAuth");
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
		ss << "Encountered error while initializing mysql connection.\n";
		ss << "Mysql error code: " << e.getErrorCode(); 
		ss << "Error Msg: " << e.what();
		throw std::runtime_error(ss.str());
	}
}

void BlogSpacePassport::HTTPPacketHandler(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept
{
	if (serverProperty.routeTable.count(request.requestPath) == 0)
	{
		RaiseHTPPError(clientfd, 404);
		LogRequestError(clientfd, request, "404");
		return;
	}



	HTTPPacket::HTTPResponsePacket response;

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

	if (request.requestPath == "/api/v1/passport/")
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::Found);
		response.SetLocation("http://blog.leaflxh.com");
		connectedClients[clientfd].writeBuffer += response.ToString();
		LogResponse(clientfd, request, response.code);

		return;
	}

	if (request.requestPath == "/api/v1/passport/login")
	{
		try
		{
			response = Login(clientfd, request);
			response.responseHeaders.insert({ "Access-Control-Allow-Origin", "http://blog.leaflxh.com" });

			connectedClients[clientfd].writeBuffer += response.ToString();
			LogResponse(clientfd, request, response.code);
		}
		catch (std::runtime_error e)
		{
			LogRequestError(clientfd, request, e.what());
		}
		return;
	}

	if (request.requestPath == "/api/v1/passport/CheckUserExist")
	{
		try
		{
			response = CheckUserExist(clientfd, request);
			response.responseHeaders.insert({ "Access-Control-Allow-Origin", "http://blog.leaflxh.com" });

			connectedClients[clientfd].writeBuffer += response.ToString();
			LogResponse(clientfd, request, response.code);
		}
		catch (std::runtime_error e)
		{
			LogRequestError(clientfd, request, e.what());
		}
		return;
	}

	if (request.requestPath == "/api/v1/passport/CheckEmailExist")
	{
		try
		{
			response = CheckEmailExist(clientfd, request);
			response.responseHeaders.insert({ "Access-Control-Allow-Origin", "http://blog.leaflxh.com" });

			connectedClients[clientfd].writeBuffer += response.ToString();
			LogResponse(clientfd, request, response.code);
		}
		catch (std::runtime_error e)
		{
			LogRequestError(clientfd, request, e.what());
		}
		return;
	}

	if (request.requestPath == "/api/v1/passport/register")
	{
		try
		{
			response = Register(clientfd, request);
			response.responseHeaders.insert({ "Access-Control-Allow-Origin", "http://blog.leaflxh.com" });

			connectedClients[clientfd].writeBuffer += response.ToString();
			LogResponse(clientfd, request, response.code);
		}
		catch (std::runtime_error e)
		{
			LogRequestError(clientfd, request, e.what());
		}
		return;
	}

	if (request.requestPath == "/api/v1/passport/SendEmailAuth")
	{
		try
		{
			response = SendEmailAuth(clientfd, request);
			response.responseHeaders.insert({ "Access-Control-Allow-Origin", "http://blog.leaflxh.com" });

			connectedClients[clientfd].writeBuffer += response.ToString();
			LogResponse(clientfd, request, response.code);
		}
		catch (std::runtime_error e)
		{
			LogRequestError(clientfd, request, e.what());
		}
		return;
	}

	if (request.requestPath == "/api/v1/passport/GetUserInfo")
	{
		try
		{
			response = GetUserInfo(clientfd, request);
			response.responseHeaders.insert({ "Access-Control-Allow-Origin", "http://blog.leaflxh.com" });

			connectedClients[clientfd].writeBuffer += response.ToString();
			LogResponse(clientfd, request, response.code);
		}
		catch (std::runtime_error e)
		{
			LogRequestError(clientfd, request, e.what());
		}
		return;
	}

	
	RaiseHTPPError(clientfd, 501);
}

HTTPPacket::HTTPResponsePacket BlogSpacePassport::CheckUserExist(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false)
{
	using boost::property_tree::ptree;
	using boost::property_tree::write_json;

	HTTPPacket::HTTPResponsePacket response;
	response.SetResponseCode(HTTPPacket::ResponseCode::OK);
	response.SetServer(SERVER_SIGNATURE);

	
	std::map<std::string, std::string> requestParam = request.ParseURLParamter();
	std::string username = requestParam["username"];
	if (request.GetContentType() == "application/x-www-form-urlencoded")
	{
		username = webstring::URLdecode(username);
	}

	try
	{
		ptree jsonTree;
		if (CheckUserExist(username) == false)
		{
			
			jsonTree.put("exist", "false");
		}
		else
		{
			jsonTree.put("exist", "true");
		}
		std::ostringstream buffer;
		write_json(buffer, jsonTree, false);

		response.SetContentType("application/json; charset=UTF-8");
		response.body = buffer.str();
	}
	catch (sql::SQLException e)
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::ServiceUnavailable);
		if (serverProperty.verbose >= VerboseLevel::essential)
		{
			LogRequestError(clientfd, request, std::string("BlogSpacePassport::CheckUserExist(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()) + ", msg: " + e.what());
		}
		else
		{
			LogRequestError(clientfd, request, std::string("BlogSpacePassport::CheckUserExist(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()));
		}
	}
	
	return response;
}

HTTPPacket::HTTPResponsePacket BlogSpacePassport::CheckEmailExist(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false)
{
	using boost::property_tree::ptree;
	using boost::property_tree::write_json;

	HTTPPacket::HTTPResponsePacket response;
	response.SetResponseCode(HTTPPacket::ResponseCode::OK);
	response.SetServer(SERVER_SIGNATURE);

	std::map<std::string, std::string> requestParam = request.ParseURLParamter();
	std::string email = requestParam["email"];
	if (request.GetContentType() == "application/x-www-form-urlencoded")
	{
		email = webstring::URLdecode(email);
	}

	try
	{
		ptree jsonTree;
		if (CheckEmailExist(email) == false)
		{

			jsonTree.put("exist", "false");
		}
		else
		{
			jsonTree.put("exist", "true");
		}
		std::ostringstream buffer;
		write_json(buffer, jsonTree, false);

		response.SetContentType("application/json; charset=UTF-8");
		response.body = buffer.str();
	}
	catch (sql::SQLException e)
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::ServiceUnavailable);
		if (serverProperty.verbose >= VerboseLevel::essential)
		{
			LogRequestError(clientfd, request, std::string("BlogSpacePassport::CheckEmailExist(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()) + ", msg: " + e.what());
		}
		else
		{
			LogRequestError(clientfd, request, std::string("BlogSpacePassport::CheckEmailExist(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()));
		}
	}

	return response;
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
	std::string password = requestParam["password"];

	if ((username == "") || (password == ""))
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::OK);

		ptree jsonTree;
		jsonTree.put("ecode", -2);
		jsonTree.put("reason", "用户名或密码错误");
		std::ostringstream buffer;
		write_json(buffer, jsonTree, false);

		response.SetContentType("application/json; charset=UTF-8");
		response.body = buffer.str();
	}
	else
	{

		if (request.GetContentType() == "application/x-www-form-urlencoded")
		{
			username = webstring::URLdecode(username);
			password = webstring::URLdecode(password);
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

				response.SetContentType("application/json; charset=UTF-8");
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

				if (authHex == webstring::sha1(password + authSalt))
				{
					std::string token = webstring::sha1(authHex + webstring::GenUUID()) + "AAAAAAAAAAAAAA";
					token += webstring::sha1(token);

					PtrPreparedStatement statement(mysqlProperty.connection->prepareStatement("INSERT INTO user_token VALUES(?,?,?)"));
					statement->setString(1, userUUID);
					statement->setString(2, token);
					statement->setString(3, webstring::GenTimeStamp());
					statement->executeUpdate();

					response.SetCookie("_sessionToken", token, 2592000, "blog.leaflxh.com", "/");
					response.SetCookie("_uuid", userUUID, 2592000, "blog.leaflxh.com", "/");

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

					response.SetContentType("application/json; charset=UTF-8");
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
	using boost::property_tree::ptree;
	using boost::property_tree::write_json;

	if (request.method != "POST")
	{
		RaiseHTPPError(clientfd, 405);
		throw std::runtime_error("requested with unspported method");
	}

	HTTPPacket::HTTPResponsePacket response;
	auto userParam = webstring::ParseKeyValue(request.body);

	//用户名
	std::string username = userParam["username"];
	//用户密码
	std::string password = userParam["password"];
	//用户邮箱
	std::string email = userParam["email"];
	//邮箱验证码
	std::string vcode = userParam["vcode"];

	if (username == "" || password == "" || email == "" || vcode == "")
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::OK);

		ptree jsonTree;
		jsonTree.put("ecode", -1);
		jsonTree.put("reason", "请求参数有误");
		std::ostringstream buffer;
		write_json(buffer, jsonTree, false);

		response.SetContentType("application/json; charset=UTF-8");
		response.body = buffer.str();
		return response;
	}

	if (request.GetContentType() == "application/x-www-form-urlencoded")
	{
		username = webstring::URLdecode(username);
		password = webstring::URLdecode(password);
		email = webstring::URLdecode(email);
		vcode = webstring::URLdecode(vcode);
	}

	if (IsVaildUserName(username) == false)
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::OK);

		ptree jsonTree;
		jsonTree.put("ecode", -5);
		jsonTree.put("reason", "用户名格式有误");
		std::ostringstream buffer;
		write_json(buffer, jsonTree, false);

		response.SetContentType("application/json; charset=UTF-8");
		response.body = buffer.str();
		return response;
	}

	if (CheckUserExist(username) == true)
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::OK);

		ptree jsonTree;
		jsonTree.put("ecode", -8);
		jsonTree.put("reason", "此用户名已被占用");
		std::ostringstream buffer;
		write_json(buffer, jsonTree, false);

		response.SetContentType("application/json; charset=UTF-8");
		response.body = buffer.str();
		return response;
	}

	if (IsVaildPassword(password) == false)
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::OK);

		ptree jsonTree;
		jsonTree.put("ecode", -6);
		jsonTree.put("reason", "密码格式有误");
		std::ostringstream buffer;
		write_json(buffer, jsonTree, false);

		response.SetContentType("application/json; charset=UTF-8");
		response.body = buffer.str();
		return response;
	}

	if (IsVaildEmailAddress(email) == false)
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::OK);

		ptree jsonTree;
		jsonTree.put("ecode", -3);
		jsonTree.put("reason", "邮箱格式有误");
		std::ostringstream buffer;
		write_json(buffer, jsonTree, false);

		response.SetContentType("application/json; charset=UTF-8");
		response.body = buffer.str();
		return response;
	}

	if (CheckEmailExist(email) == true)
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::OK);

		ptree jsonTree;
		jsonTree.put("ecode", -9);
		jsonTree.put("reason", "此邮箱已被占用");
		std::ostringstream buffer;
		write_json(buffer, jsonTree, false);

		response.SetContentType("application/json; charset=UTF-8");
		response.body = buffer.str();
		return response;
	}

	
	try
	{
		mysqlProperty.connection->setSchema("lxhblogspace_passport");
		PtrPreparedStatement statement(mysqlProperty.connection->prepareStatement("SELECT email_code, code_date FROM user_email_code where email = ?"));
		statement->setString(1, email);
		PtrResultSet result(statement->executeQuery());

		bool found = false;
		while (result->next())
		{
			/*email_code*/
			if (vcode == result->getString(1))
			{
				/*code_date*/
				if ((time(nullptr) - result->getInt64(2)) < 120)
				{
					found = true;
				}
				break;
			}
		}

		if (found == false)
		{
			response.SetResponseCode(HTTPPacket::ResponseCode::OK);

			ptree jsonTree;
			jsonTree.put("ecode", -7);
			jsonTree.put("reason", "提供的邮件验证码无效或已过期");
			std::ostringstream buffer;
			write_json(buffer, jsonTree, false);

			response.SetContentType("application/json; charset=UTF-8");
			response.body = buffer.str();
			return response;
		}
	}
	catch (sql::SQLException e)
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::ServiceUnavailable);
		if (serverProperty.verbose >= VerboseLevel::essential)
		{
			LogRequestError(clientfd, request, std::string("BlogSpacePassport::SendEmailAuth(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()) + ", msg: " + e.what());
		}
		else
		{
			LogRequestError(clientfd, request, std::string("BlogSpacePassport::SendEmailAuth(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()));
		}
		return response;
	}

	//验证完毕，添加用户
	try
	{
		//使用事务来保证用户数据的完整性
		mysqlProperty.connection->setSchema("lxhblogspace_passport");

		PtrStatement transaction(mysqlProperty.connection->createStatement());
		transaction->execute("BEGIN");

		std::string uuid = webstring::GenUUID();
		std::string salt = webstring::GenUUID();

		PtrPreparedStatement user_info(mysqlProperty.connection->prepareStatement("INSERT INTO user_info VALUES(?,?,?,?,?)"));
		user_info->setString(1, uuid);
		user_info->setInt64(2, time(nullptr));
		user_info->setString(3, email);
		user_info->setString(4, username);
		user_info->setInt(5, 0);
		user_info->executeUpdate();

		PtrPreparedStatement user_details(mysqlProperty.connection->prepareStatement("INSERT INTO user_details VALUES(?,?,?)"));
		user_details->setString(1, uuid);
		user_details->setString(2, "/images/default_avatar.jpg");
		user_details->setString(3, "");
		user_details->executeUpdate();

		PtrPreparedStatement user_auth(mysqlProperty.connection->prepareStatement("INSERT INTO user_auth VALUES(?,?,?)"));
		user_auth->setString(1, uuid);
		user_auth->setString(2, webstring::sha1(password + salt));
		user_auth->setString(3, salt);
		user_auth->executeUpdate();

		transaction->execute("commit");

		response.SetResponseCode(HTTPPacket::ResponseCode::OK);

		ptree jsonTree;
		jsonTree.put("ecode", 0);
		jsonTree.put("reason", "注册成功");
		std::ostringstream buffer;
		write_json(buffer, jsonTree, false);

		response.SetContentType("application/json; charset=UTF-8");
		response.body = buffer.str();
	}
	catch (sql::SQLException e)
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::ServiceUnavailable);
		if (serverProperty.verbose >= VerboseLevel::essential)
		{
			LogRequestError(clientfd, request, std::string("BlogSpacePassport::SendEmailAuth(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()) + ", msg: " + e.what());
		}
		else
		{
			LogRequestError(clientfd, request, std::string("BlogSpacePassport::SendEmailAuth(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()));
		}
		return response;
	}

	return response;
	
}

//缺少一个验证码发送间隔的限制
HTTPPacket::HTTPResponsePacket BlogSpacePassport::SendEmailAuth(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false)
{
	using boost::property_tree::ptree;
	using boost::property_tree::write_json;

	if (request.method != "POST")
	{
		RaiseHTPPError(clientfd, 405);
		throw std::runtime_error("requested with unspported method");
	}


	HTTPPacket::HTTPResponsePacket response;
	auto userParam = webstring::ParseKeyValue(request.body);
	//用户邮箱地址
	std::string userEmailAddress = userParam["email"];
	//用户输入的图形验证码
	std::string verifyingCode = userParam["verification"];

	if (request.GetContentType() == "application/x-www-form-urlencoded")
	{
		userEmailAddress = webstring::URLdecode(userEmailAddress);
		verifyingCode = webstring::URLdecode(verifyingCode);
	}

	//检查邮箱格式
	if (IsVaildEmailAddress(userEmailAddress) == false)
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::OK);

		ptree jsonTree;
		jsonTree.put("ecode", -3);
		jsonTree.put("reason", "邮箱格式有误");
		std::ostringstream buffer;
		write_json(buffer, jsonTree, false);

		response.SetContentType("application/json; charset=UTF-8");
		response.body = buffer.str();
		return response;
	}

	//生成验证码，并存储至数据库
	unsigned long seed = time(nullptr);
	for (int i = 0; i < userEmailAddress.length() % 10; ++i)
	{
		seed += userEmailAddress[i] + i;
	}
	srand(seed);

	std::string code;
	std::stringstream stream;
	stream << std::setw(8) << std::setfill('0') << (rand() % 99999999);
	stream >> code;
	try
	{
		mysqlProperty.connection->setSchema("lxhblogspace_passport");
		PtrPreparedStatement statement(mysqlProperty.connection->prepareStatement("INSERT INTO user_email_code VALUES(?,?,?)"));
		statement->setString(1, userEmailAddress);
		statement->setString(2, code);
		statement->setInt64(3, time(nullptr));

		statement->executeUpdate();
		
		//此处的指针交由AsyncSendEmail函数释放
		SSLEmailService *service = new SSLEmailService();
		service->email.sender = "LXHBlogSpace";
		service->email.senderEmail = emailProperty.sender.data();
		service->email.password = emailProperty.auth.data();
		service->email.smtpServer = emailProperty.server.data();
		service->email.serverPort = emailProperty.port.data();
		service->email.charset = "utf-8";
		service->email.recipient = "用户";
		service->email.recipientEmail = userEmailAddress.data();

		service->email.subject = "LXHBlogSpace--邮箱验证码";
		service->email.MIMEContentType = "text/html";
		service->email.message = "邮箱验证码：";


		stream.clear();
		stream.str("");
		stream << "<span style=\"font-weight:800;color:#3140b9\">" << code << "</span>" << ", 2分钟内有效";
		service->email.message += stream.str();
		
		pthread_t tid;
		pthread_create(&tid, 0, AsyncSendEmail, (void*)service);

		response.SetResponseCode(HTTPPacket::ResponseCode::OK);
		ptree jsonTree;
		jsonTree.put("ecode", 0);
		jsonTree.put("reason", "认证码发送成功");
		std::ostringstream buffer;
		write_json(buffer, jsonTree, false);

		response.SetContentType("application/json; charset=UTF-8");
		response.body = buffer.str();
	}
	catch (sql::SQLException e)
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::ServiceUnavailable);
		if (serverProperty.verbose >= VerboseLevel::essential)
		{
			LogRequestError(clientfd, request, std::string("BlogSpacePassport::SendEmailAuth(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()) + ", msg: " + e.what());
		}
		else
		{
			LogRequestError(clientfd, request, std::string("BlogSpacePassport::SendEmailAuth(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()));
		}
	}

	return response;
}

HTTPPacket::HTTPResponsePacket BlogSpacePassport::GetUserInfo(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false)
{
	HTTPPacket::HTTPResponsePacket response;
	response.SetResponseCode(HTTPPacket::ResponseCode::OK);
	response.SetServer(SERVER_SIGNATURE);
	response.SetKeepAlive(serverProperty.timeout, serverProperty.maxRequestsNum);

	std::string uuid = request.GetCookieValue("_uuid");
	std::string token = request.GetCookieValue("_sessionToken");

	std::map<std::string, std::string> resultJson;
	resultJson["vaild"] = "false";
	response.SetContentType("application/json; charset=UTF-8");

	if (uuid == "" || token == "")
	{
		response.body = webstring::JsonStringify(resultJson);
		return response;
	}

	try
	{
		mysqlProperty.connection->setSchema("lxhblogspace_passport");
		PtrPreparedStatement statement(mysqlProperty.connection->prepareStatement("SELECT user_uuid, token_value, token_date FROM user_token WHERE user_uuid = ?"));
		statement->setString(1, uuid);
		PtrResultSet result(statement->executeQuery());

		bool vaild = false;
		while (result->next())
		{
			if (result->getString(1) == uuid)
			{
				if (result->getString(2) == token)
				{
					//30天有效期
					if ((time(nullptr) - result->getInt64(3)) < 2592000)
					{
						vaild = true;
						break;
					}
				}
			}
		}

		if (vaild == false)
		{
			response.body = webstring::JsonStringify(resultJson);
			return response;
		}

		statement.reset(mysqlProperty.connection->prepareStatement("SELECT avatar, description FROM user_details where user_uuid = ?"));
		statement->setString(1, uuid);
		result.reset(statement->executeQuery());
		while (result->next())
		{
			resultJson["vaild"] = "true";
			resultJson["avatar"] = result->getString(1);
			resultJson["description"] = result->getString(2);
		}

		statement.reset(mysqlProperty.connection->prepareStatement("SELECT register_date, email, username, is_locked FROM user_info where user_uuid = ?"));
		statement->setString(1, uuid);
		result.reset(statement->executeQuery());
		while (result->next())
		{
			resultJson["register_date"] = result->getString(1);
			resultJson["email"] = result->getString(2);
			resultJson["username"] = result->getString(3);
			resultJson["is_locked"] = result->getString(4);
		}

		response.body = webstring::JsonStringify(resultJson);
	}
	catch (sql::SQLException e)
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::ServiceUnavailable);
		if (serverProperty.verbose >= VerboseLevel::essential)
		{
			LogRequestError(clientfd, request, std::string("BlogSpacePassport::GetUserInfo(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()) + ", msg: " + e.what());
		}
		else
		{
			LogRequestError(clientfd, request, std::string("BlogSpacePassport::GetUserInfo(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()));
		}
	}

	return response;
}

bool BlogSpacePassport::CheckUserExist(std::string username) noexcept(false)
{
	std::string userUUID;

	mysqlProperty.connection->setSchema("lxhblogspace_passport");
	PtrPreparedStatement statement(mysqlProperty.connection->prepareStatement("SELECT user_uuid FROM user_info WHERE username = ?"));

	statement->setString(1, username);
	PtrResultSet queryResult(statement->executeQuery());
	while (queryResult->next())
	{
		userUUID = queryResult->getString(1);
		break;
	}

	return !(userUUID == "");
}

bool BlogSpacePassport::CheckEmailExist(std::string email) noexcept(false)
{
	std::string userUUID;

	mysqlProperty.connection->setSchema("lxhblogspace_passport");
	PtrPreparedStatement statement(mysqlProperty.connection->prepareStatement("SELECT user_uuid FROM user_info WHERE email = ?"));

	statement->setString(1, email);
	PtrResultSet queryResult(statement->executeQuery());
	while (queryResult->next())
	{
		userUUID = queryResult->getString(1);
		break;
	}

	return !(userUUID == "");
}

bool BlogSpacePassport::IsVaildUserName(std::string username) noexcept
{
	//缺少utf8编码的验证
	return (webstring::UTF8Strlen(username) <= 10);
}

bool BlogSpacePassport::IsVaildPassword(std::string password) noexcept
{
	//长度为8-15位，需为字母和数字的组合，可添加下划线
	if (password.length() >= 8 && password.length() <= 15)
	{
		bool foundAlpha = false;
		bool foundDigit = false;
		for (auto chr : password)
		{
			if ((chr >= 'A' && chr <= 'Z') || (chr >= 'a' && chr <= 'z'))
			{
				foundAlpha = true;
			}
			else if (chr >= '0' && chr <= '9')
			{
				foundDigit = true;
			}
			else if (chr == '_')
			{
				continue;
			}
			else
			{
				return false;
			}
		}

		if (foundAlpha == false || foundDigit == false)
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	return false;
}

bool BlogSpacePassport::IsVaildEmailAddress(std::string email) noexcept
{
	if (email != "")
	{
		//邮箱名允许数字、字母、英文句点、加号、减号，下划线，邮件提供商允许QQ邮箱，网易126、163邮箱、谷歌gamil邮箱、foxmail邮箱、新浪邮箱
		std::regex format("[0-9a-zA-Z\\.\\+\\-_]+?@[0-9a-zA-Z\\.]*?(qq\\.com|163\\.com|gmail\\.com|foxmail\\.com|126\\.com|sina\\.com)");
		if (email.length() < 30 && std::regex_match(email, format))
		{
			return true;
		}
	}

	return false;
}