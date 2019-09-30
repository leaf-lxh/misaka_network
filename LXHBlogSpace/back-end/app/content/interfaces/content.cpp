#include "content.h"
#include <tinyHttpd/webstring.h>

#include <sstream>
#include <fstream>

#define SERVER_SIGNATURE "tinyHttpd/0.1a"

void BlogSpaceContent::ModuleInit() noexcept(false)
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

	//初始化文件存放根目录
	auto imageRootDirIter = setting.find("img_root_dir");
	if (imageRootDirIter == setting.end())
	{
		throw runtime_error("You must specify the image storing root path with /img_root_dir/ variable.");
	}
	else
	{
		imageRootPath = webstring::strip(imageRootDirIter->second, "\"\' ");
	}

	//其他的初始化
	InitRouteTabel();
	InitMysqlConnection();
}

void BlogSpaceContent::InitRouteTabel() noexcept
{
	AddRoutePath("/api/v1/content/");
	AddRoutePath("/api/v1/content/UploadImage");
	AddRoutePath("/api/v1/content/SaveDraft");
	AddRoutePath("/api/v1/content/ReadDraft");
	AddRoutePath("/api/v1/content/PublishArticle");
	AddRoutePath("/api/v1/content/GetArticleContent");
	AddRoutePath("/api/v1/content/DeleteArticle");
}

void BlogSpaceContent::InitMysqlConnection() noexcept(false)
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

void BlogSpaceContent::HTTPPacketHandler(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept
{
	if (serverProperty.routeTable.count(request.requestPath) == 0)
	{
		RaiseHTPPError(clientfd, 404);
		return;
	}

	HTTPPacket::HTTPResponsePacket response;

	if (request.requestPath == "/api/v1/content/")
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::Found);
		response.SetLocation("http://blog.leaflxh.com");
		connectedClients[clientfd].writeBuffer += response.ToString();
		LogResponse(clientfd, request, response.code);

		return;
	}

	if (request.requestPath == "/api/v1/content/UploadImage")
	{
		try
		{
			response = UploadImage(clientfd, request);
			response.responseHeaders.insert({ "Access-Control-Allow-Origin", "http://localhost:4563" });

			connectedClients[clientfd].writeBuffer += response.ToString();
			LogResponse(clientfd, request, response.code);
		}
		catch (std::runtime_error e)
		{
			LogRequestError(clientfd, request, e.what());
		}
		return;
	}
}

HTTPPacket::HTTPResponsePacket BlogSpaceContent::UploadImage(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false)
{
	HTTPPacket::HTTPResponsePacket response;
	response.SetServer(SERVER_SIGNATURE);
	std::map<std::string, std::string> responseJson;

	//是否存在文件头的必要条件
	if (request.body.length() < 2)
	{
		return response;
	}

	//检查用户是否登录，是否被封禁
	try
	{
		if (CheckUserToken(request) == false)
		{
			RaiseHTPPError(clientfd, 403);
			throw std::runtime_error("BlogSpaceContent::UploadImage(): Access denied");
		}
	}
	catch (sql::SQLException e)
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::ServiceUnavailable);
		if (serverProperty.verbose >= VerboseLevel::essential)
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::UploadImage: MySQL exception occured with code ") + std::to_string(e.getErrorCode()) + ", msg: " + e.what());
		}
		else
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::UploadImage: MySQL exception occured with code ") + std::to_string(e.getErrorCode()));
		}

		return response;
	}
	

	//检查文件头部，确定文件类型。只允许JPEG, PNG, BMP格式的文件
	std::string fileHeader = request.body.substr(0, 2);
	std::cout << std::hex << int((char)fileHeader[0]) << std::endl;
	std::cout << std::hex << int((char)fileHeader[1]) << std::endl;
	std::string fileExtention;
	if (fileHeader == "\xFF\xD8")
	{
		fileExtention = ".jpg";
	}
	else if (fileHeader == "\x42\x4d")
	{
		fileExtention = ".png";
	}
	else if (fileHeader == "\x89\x50")
	{
		fileExtention = ".bmp";
	}
	else
	{
		responseJson.insert({ "ecode", "-1" });
		responseJson.insert({ "reason", "文件格式错误，只接受png，jpg，以及bmp格式的图片" });

		response.body = webstring::JsonStringify(responseJson);
		response.SetContentType("application/json; charset=UTF-8");
		return response;
	}

	//存储文件
	std::string filepath;
	try
	{
		filepath = PrepareDirectory(fileExtention);
	}
	catch (std::runtime_error e)
	{
		RaiseHTPPError(clientfd, 500);
		throw std::runtime_error("BlogSpaceContent::UploadImage(): Preparing directory failed");
	}
	
	std::fstream file(imageRootPath + filepath, std::fstream::out);
	if (file.good() != true)
	{
		RaiseHTPPError(clientfd, 500);
		throw std::runtime_error("BlogSpaceContent::UploadImage(): file.good() == false");
	}

	file.write(request.body.c_str(), request.body.length());
	responseJson.insert({ "ecode", "0" });
	responseJson.insert({ "reason", "上传成功" });
	responseJson.insert({ "img_path", filepath });

	//将文件路径存放至数据库，用于追责
	try
	{
		mysqlProperty.connection->setSchema("lxhblogspace_content");
		PtrPreparedStatement statement(mysqlProperty.connection->prepareStatement("INSERT INTO image_path VALUES(?,?,?)"));
		statement->setString(1, request.GetCookieValue("_uuid"));
		statement->setString(2, webstring::GenTimeStamp());
		statement->setString(3, request.GetCookieValue("_sessionToken"));
		statement->execute();
	}
	catch (sql::SQLException e)
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::ServiceUnavailable);
		if (serverProperty.verbose >= VerboseLevel::essential)
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::UploadImage: MySQL exception occured with code ") + std::to_string(e.getErrorCode()) + ", msg: " + e.what());
		}
		else
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::UploadImage: MySQL exception occured with code ") + std::to_string(e.getErrorCode()));
		}

		return response;
	}

	response.SetContentType("application/json; charset=UTF-8");
	response.body = webstring::JsonStringify(responseJson);
	return response;
}

bool BlogSpaceContent::CheckUserToken(HTTPPacket::HTTPRequestPacket &request) noexcept(false)
{
	std::string uuid = request.GetCookieValue("_uuid");
	std::string token = request.GetCookieValue("_sessionToken");

	if (uuid == "" || token == "")
	{
		return false;
	}

	bool logined = false;
	bool locked = true;
	mysqlProperty.connection->setSchema("lxhblogspace_passport");
	PtrPreparedStatement statement(mysqlProperty.connection->prepareStatement("SELECT token_value, token_date FROM user_token WHERE user_uuid = ?"));
	statement->setString(1, uuid);

	PtrResultSet result(statement->executeQuery());
	while (result->next())
	{
		if (result->getString(1) == token)
		{
			//30天有效期
			if ((time(nullptr) - result->getInt64(2)) < 2592000)
			{
				logined = true;
				break;
			}
		}
	}

	if (logined = false)
	{
		return false;
	}

	statement.reset(mysqlProperty.connection->prepareStatement("SELECT is_locked FROM user_info WHERE user_uuid = ?"));
	statement->setString(1, uuid);
	result.reset(statement->executeQuery());
	while (result->next())
	{
		if (result->getInt(1) != 0)
		{
			return false;
		}
	}

	return true;
}

std::string BlogSpaceContent::PrepareDirectory(std::string fileExtention) noexcept(false)
{
	for (int i = 0; i < 10; ++i)
	{
		std::string filepath = "/images/";
		filepath += webstring::sha1(webstring::GenUUID()) + fileExtention;

		if (std::fstream(imageRootPath + filepath, std::fstream::in).good() == false)
		{
			return filepath;
		}
	}
	
	throw std::runtime_error("建议您今天购买彩票。");
}

