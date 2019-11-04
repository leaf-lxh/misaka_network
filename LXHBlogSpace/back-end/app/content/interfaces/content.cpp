#include "content.h"
#include <tinyHttpd/webstring.h>

#include <sstream>
#include <fstream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <regex>

#include <nlohmann/json.hpp>

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
	InitMysqlConnection();
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

	if (request.requestPath == "/api/v1/content/UploadImage")
	{
		try
		{
			response = UploadImage(clientfd, request);
			
			connectedClients[clientfd].writeBuffer += response.ToString();
			LogResponse(clientfd, request, response.code);
		}
		catch (std::runtime_error e)
		{
			LogRequestError(clientfd, request, e.what());
		}
		return;
	}

	if (request.requestPath == "/api/v1/content/SaveDraft")
	{
		try
		{
			response = SaveDraft(clientfd, request);
			
			connectedClients[clientfd].writeBuffer += response.ToString();
			LogResponse(clientfd, request, response.code);
		}
		catch (std::runtime_error e)
		{
			LogRequestError(clientfd, request, e.what());
		}
		return;
	}

	if (request.requestPath == "/api/v1/content/ReadDraft")
	{
		try
		{
			response = ReadDraft(clientfd, request);

			connectedClients[clientfd].writeBuffer += response.ToString();
			LogResponse(clientfd, request, response.code);
		}
		catch (std::runtime_error e)
		{
			LogRequestError(clientfd, request, e.what());
		}
		return;
	}

	if (request.requestPath == "/api/v1/content/DeleteDraft")
	{
		try
		{
			response = DeleteDraft(clientfd, request);

			connectedClients[clientfd].writeBuffer += response.ToString();
			LogResponse(clientfd, request, response.code);
		}
		catch (std::runtime_error e)
		{
			LogRequestError(clientfd, request, e.what());
		}
		return;
	}

	if (request.requestPath == "/api/v1/content/GetDraftList")
	{
		try
		{
			response = GetDraftList(clientfd, request);

			connectedClients[clientfd].writeBuffer += response.ToString();
			LogResponse(clientfd, request, response.code);
		}
		catch (std::runtime_error e)
		{
			LogRequestError(clientfd, request, e.what());
		}
		return;
	}

	if (request.requestPath == "/api/v1/content/PublishArticle")
	{
		try
		{
			response = PublishArticle(clientfd, request);

			connectedClients[clientfd].writeBuffer += response.ToString();
			LogResponse(clientfd, request, response.code);
		}
		catch (std::runtime_error e)
		{
			LogRequestError(clientfd, request, e.what());
		}
		return;
	}

	if (request.requestPath == "/api/v1/content/GetArticleContent")
	{
		try
		{
			response = GetArticleContent(clientfd, request);

			connectedClients[clientfd].writeBuffer += response.ToString();
			LogResponse(clientfd, request, response.code);
		}
		catch (std::runtime_error e)
		{
			LogRequestError(clientfd, request, e.what());
		}
		return;
	}

	if (request.requestPath == "/api/v1/content/DeleteArticle")
	{
		try
		{
			response = DeleteArticle(clientfd, request);

			connectedClients[clientfd].writeBuffer += response.ToString();
			LogResponse(clientfd, request, response.code);
		}
		catch (std::runtime_error e)
		{
			LogRequestError(clientfd, request, e.what());
		}
		return;
	}
	
	if (request.requestPath == "/api/v1/content/GetSubscribedList")
	{
		try
		{
			response = GetSubscribedList(clientfd, request);

			connectedClients[clientfd].writeBuffer += response.ToString();
			LogResponse(clientfd, request, response.code);
		}
		catch (std::runtime_error e)
		{
			LogRequestError(clientfd, request, e.what());
		}
		return;
	}

	if (request.requestPath == "/api/v1/content/GetUserArticleList")
	{
		try
		{
			response = GetUserArticleList(clientfd, request);

			connectedClients[clientfd].writeBuffer += response.ToString();
			LogResponse(clientfd, request, response.code);
		}
		catch (std::runtime_error e)
		{
			LogRequestError(clientfd, request, e.what());
		}
		return;
	}

	if (request.requestPath == "/api/v1/content/vote")
	{
		try
		{
			response = Vote(clientfd, request);

			connectedClients[clientfd].writeBuffer += response.ToString();
			LogResponse(clientfd, request, response.code);
		}
		catch (std::runtime_error e)
		{
			LogRequestError(clientfd, request, e.what());
		}
		return;
	}

	if (request.requestPath == "/api/v1/content/subscribe")
	{
		try
		{
			response = Subscribe(clientfd, request);

			connectedClients[clientfd].writeBuffer += response.ToString();
			LogResponse(clientfd, request, response.code);
		}
		catch (std::runtime_error e)
		{
			LogRequestError(clientfd, request, e.what());
		}
		return;
	}

	if (request.requestPath == "/api/v1/content/CheckUserOperation")
	{
		try
		{
			response = CheckUserOperation(clientfd, request);

			connectedClients[clientfd].writeBuffer += response.ToString();
			LogResponse(clientfd, request, response.code);
		}
		catch (std::runtime_error e)
		{
			LogRequestError(clientfd, request, e.what());
		}
		return;
	}

	if (request.requestPath == "/api/v1/content/SendComment")
	{
		try
		{
			response = SendComment(clientfd, request);

			connectedClients[clientfd].writeBuffer += response.ToString();
			LogResponse(clientfd, request, response.code);
		}
		catch (std::runtime_error e)
		{
			LogRequestError(clientfd, request, e.what());
		}
		return;
	}

	if (request.requestPath == "/api/v1/content/GetComments")
	{
		try
		{
			response = GetComments(clientfd, request);

			connectedClients[clientfd].writeBuffer += response.ToString();
			LogResponse(clientfd, request, response.code);
		}
		catch (std::runtime_error e)
		{
			LogRequestError(clientfd, request, e.what());
		}
		return;
	}

	if (request.requestPath == "/api/v1/content/GetPublishArticleList")
	{
		try
		{
			response = GetPublishArticleList(clientfd, request);

			connectedClients[clientfd].writeBuffer += response.ToString();
			LogResponse(clientfd, request, response.code);
		}
		catch (std::runtime_error e)
		{
			LogRequestError(clientfd, request, e.what());
		}
		return;
	}

	if (request.requestPath == "/api/v1/content/GetFollowedArticleList")
	{
		try
		{
			response = GetFollowedArticleList(clientfd, request);

			connectedClients[clientfd].writeBuffer += response.ToString();
			LogResponse(clientfd, request, response.code);
		}
		catch (std::runtime_error e)
		{
			LogRequestError(clientfd, request, e.what());
		}
		return;
	}


	if (request.requestPath == "/api/v1/content/GetHottopic")
	{
		try
		{
			response = GetHottopic(clientfd, request);

			connectedClients[clientfd].writeBuffer += response.ToString();
			LogResponse(clientfd, request, response.code);
		}
		catch (std::runtime_error e)
		{
			LogRequestError(clientfd, request, e.what());
		}
		return;
	}

	if (request.requestPath == "/api/v1/content/GetFriendLink")
	{
		try
		{
			response = GetFriendLink(clientfd, request);

			connectedClients[clientfd].writeBuffer += response.ToString();
			LogResponse(clientfd, request, response.code);
		}
		catch (std::runtime_error e)
		{
			LogRequestError(clientfd, request, e.what());
		}
		return;
	}

	response.SetResponseCode(HTTPPacket::ResponseCode::NotFound);
	connectedClients[clientfd].writeBuffer += response.ToString();
	LogResponse(clientfd, request, response.code);

	
	return;
}

HTTPPacket::HTTPResponsePacket BlogSpaceContent::UploadImage(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false)
{
	
	HTTPPacket::HTTPResponsePacket response;
	response.SetServer(SERVER_SIGNATURE);

	if (request.method != "POST")
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::MethodNotAllowed);
		return response;
	}

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
			response.SetResponseCode(HTTPPacket::ResponseCode::Forbidden);
			return response;
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
	std::string fileExtention;
	if (fileHeader == "\xFF\xD8")
	{
		fileExtention = ".jpg";
	}
	else if (fileHeader == "\x42\x4d")
	{
		fileExtention = ".bmp";
	}
	else if (fileHeader == "\x89\x50")
	{
		fileExtention = ".png";
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
		statement->setString(3, filepath);
		statement->execute();
	}
	catch (sql::SQLException e)
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::ServiceUnavailable);
		if (serverProperty.verbose >= VerboseLevel::essential)
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::UploadImage(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()) + ", msg: " + e.what());
		}
		else
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::UploadImage(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()));
		}

		return response;
	}

	response.SetContentType("application/json; charset=UTF-8");
	response.body = webstring::JsonStringify(responseJson);

	return response;
}

HTTPPacket::HTTPResponsePacket BlogSpaceContent::SaveDraft(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false)
{
	HTTPPacket::HTTPResponsePacket response;
	response.SetServer(SERVER_SIGNATURE);

	if (request.method != "POST")
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::MethodNotAllowed);
		return response;
	}

	std::map<std::string, std::string> responseJson;

	try
	{
		if (CheckUserToken(request) == false)
		{
			response.SetResponseCode(HTTPPacket::ResponseCode::Forbidden);
			return response;
		}
	}
	catch (sql::SQLException e)
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::ServiceUnavailable);
		if (serverProperty.verbose >= VerboseLevel::essential)
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::SaveDraft(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()) + ", msg: " + e.what());
		}
		else
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::SaveDraft(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()));
		}

		return response;
	}
	
	auto postParamaters = webstring::ParseKeyValue(request.body);
	if (postParamaters.count("bg") == 0 || postParamaters.count("imgs") == 0 || postParamaters.count("title") == 0 || postParamaters.count("content") == 0)
	{
		responseJson.insert({ "ecode", "-2" });
		responseJson.insert({ "reason", "请求参数有误" });

		response.SetContentType("application/json; charset=UTF-8");
		response.body = webstring::JsonStringify(responseJson);
		return response;
	}

	std::string draftid = postParamaters["draft_id"];
	std::string backgroundPath = webstring::URLdecode(postParamaters["bg"]);
	std::string images = webstring::URLdecode(postParamaters["imgs"]);
	std::string draftTitle = postParamaters["title"];
	std::string draftContent = postParamaters["content"];
	
	if (draftTitle.length() > 0)
	{
		draftTitle = webstring::Base64Decode(draftTitle);
	}

	if (draftContent.length() > 0)
	{
		draftContent = webstring::Base64Decode(draftContent);
	}

	if (webstring::UTF8Strlen(draftTitle) > 50 || webstring::UTF8Strlen(draftContent) >50000)
	{
		responseJson.insert({ "ecode", "-3" });
		responseJson.insert({ "reason", "文章标题或内容过长" });

		response.SetContentType("application/json; charset=UTF-8");
		response.body = webstring::JsonStringify(responseJson);
		return response;
	}

	try
	{
		//如果draftid不为空，则更新草稿
		if (draftid != "")
		{
			mysqlProperty.connection->setSchema("lxhblogspace_content");
			PtrPreparedStatement statement(mysqlProperty.connection->prepareStatement("SELECT draft_id, deleted FROM draft_info WHERE user_uuid=?"));
			statement->setString(1, request.GetCookieValue("_uuid"));
			PtrResultSet result(statement->executeQuery());
			while (result->next())
			{
				if (result->getString(1) == draftid)
				{
					if (result->getUInt("deleted") == 1)
					{
						break;
					}

					PtrStatement transaction(mysqlProperty.connection->createStatement());
					transaction->execute("BEGIN");

					statement.reset(mysqlProperty.connection->prepareStatement("UPDATE draft_info SET lastmodify_date=?, background_img=?, saved_images=? WHERE draft_id=?"));
					statement->setString(1, webstring::GenTimeStamp());
					statement->setString(2, backgroundPath);
					statement->setString(3, images);
					statement->setString(4, draftid);
					statement->executeUpdate();

					statement.reset(mysqlProperty.connection->prepareStatement("UPDATE draft_content SET title=?, content=? WHERE draft_id=?"));
					statement->setString(1, draftTitle);
					statement->setString(2, draftContent);
					statement->setString(3, draftid);
					statement->executeUpdate();

					transaction->execute("COMMIT");

					responseJson.insert({ "ecode", "0" });
					responseJson.insert({ "reason", "保存成功" });
					responseJson.insert({ "draft_id", draftid });

					response.SetContentType("application/json; charset=UTF-8");
					response.body = webstring::JsonStringify(responseJson);
					return response;
				}
			}

			responseJson.insert({ "ecode", "-4" });
			responseJson.insert({ "reason", "指定草稿不存在" });
			responseJson.insert({ "draft_id", draftid });

			response.SetContentType("application/json; charset=UTF-8");
			response.body = webstring::JsonStringify(responseJson);
			return response;
		}

		PtrStatement transaction(mysqlProperty.connection->createStatement());
		transaction->execute("BEGIN");

		mysqlProperty.connection->setSchema("lxhblogspace_content");
		PtrPreparedStatement statement(mysqlProperty.connection->prepareStatement("INSERT INTO draft_info(user_uuid, create_date, lastmodify_date, background_img, saved_images, deleted) VALUES(?,?,?,?,?,?)"));

		statement->setString(1, request.GetCookieValue("_uuid"));
		statement->setString(2, webstring::GenTimeStamp());
		statement->setString(3, webstring::GenTimeStamp());
		statement->setString(4, backgroundPath);
		statement->setString(5, images);
		statement->setUInt(6, 0);
		statement->executeUpdate();

		std::string insertID;
		statement.reset(mysqlProperty.connection->prepareStatement("SELECT LAST_INSERT_ID()"));
		PtrResultSet result(statement->executeQuery());
		while (result->next())
		{
			insertID = result->getString(1);
		}

		statement.reset(mysqlProperty.connection->prepareStatement("INSERT INTO draft_content VALUES(?,?,?)"));
		statement->setString(1, insertID);
		statement->setString(2, draftTitle);
		statement->setString(3, draftContent);
		statement->executeUpdate();

		transaction->execute("COMMIT");

		responseJson.insert({ "ecode", "0" });
		responseJson.insert({ "reason", "保存成功" });
		responseJson.insert({ "draft_id", insertID });

		response.SetContentType("application/json; charset=UTF-8");
		response.body = webstring::JsonStringify(responseJson);
	}
	catch (sql::SQLException e)
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::ServiceUnavailable);
		if (serverProperty.verbose >= VerboseLevel::essential)
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::SaveDraft(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()) + ", msg: " + e.what());
		}
		else
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::SaveDraft(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()));
		}

		return response;
	}


	return response;
}

HTTPPacket::HTTPResponsePacket BlogSpaceContent::ReadDraft(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false)
{
	HTTPPacket::HTTPResponsePacket response;
	response.SetServer(SERVER_SIGNATURE);

	if (request.method != "GET")
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::MethodNotAllowed);
		return response;
	}

	std::map<std::string, std::string> responseJson;

	try
	{
		if (CheckUserToken(request) == false)
		{
			RaiseHTPPError(clientfd, 403);
			throw std::runtime_error("BlogSpaceContent::ReadDraft(): Access denied");
		}
	}
	catch (sql::SQLException e)
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::ServiceUnavailable);
		if (serverProperty.verbose >= VerboseLevel::essential)
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::ReadDraft(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()) + ", msg: " + e.what());
		}
		else
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::ReadDraft(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()));
		}

		return response;
	}

	auto postParamaters = request.ParseURLParamter();
	if (postParamaters["draft_id"] == "")
	{
		responseJson["ecode"] = "-5";
		responseJson["reason"] = "请求参数有误";

		response.SetContentType("application/json; charset=UTF-8");
		response.body = webstring::JsonStringify(responseJson);
		return response;
	}

	std::string draftId = postParamaters["draft_id"];
	std::string user_uuid = request.GetCookieValue("_uuid");

	try
	{
		mysqlProperty.connection->setSchema("lxhblogspace_content");
		PtrPreparedStatement statement(mysqlProperty.connection->prepareStatement("SELECT user_uuid, lastmodify_date, background_img, saved_images, deleted FROM draft_info WHERE draft_id=?"));
		statement->setString(1, draftId);
		PtrResultSet result(statement->executeQuery());
		while (result->next())
		{
			if (user_uuid == result->getString(1))
			{
				if (result->getUInt("deleted") == 1)
				{
					break;
				}

				//如果是当前用户的草稿，进行数据的读取
				responseJson["draft_id"] = draftId;
				responseJson["lastmodify_time"] = result->getString(2);
				responseJson["bg"] = webstring::URLencode(result->getString(3));
				responseJson["imgs"] = webstring::URLencode(result->getString(4));

				statement.reset(mysqlProperty.connection->prepareStatement("SELECT title, content FROM draft_content WHERE draft_id=?"));
				statement->setString(1, draftId);
				result.reset(statement->executeQuery());
				while (result->next())
				{
					responseJson["title"] = webstring::Base64Encode(result->getString(1));
					responseJson["content"] = webstring::Base64Encode(result->getString(2));
				}

				responseJson["ecode"] = "0";
				response.SetContentType("application/json; charset=UTF-8");
				response.body = webstring::JsonStringify(responseJson);
				return response;

			}
			else
			{
				//不是当前用户则无权限读取
				response.SetResponseCode(HTTPPacket::ResponseCode::Forbidden);
				return response;
			}
		}

		//执行至此说明指定的draftId不存在
		responseJson["ecode"] = "-4";
		responseJson["reason"] = "指定草稿不存在";

		response.SetContentType("application/json; charset=UTF-8");
		response.body = webstring::JsonStringify(responseJson);
		return response;

	}
	catch (sql::SQLException e)
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::ServiceUnavailable);
		if (serverProperty.verbose >= VerboseLevel::essential)
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::ReadDraft(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()) + ", msg: " + e.what());
		}
		else
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::ReadDraft(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()));
		}

		return response;
	}
}

HTTPPacket::HTTPResponsePacket BlogSpaceContent::DeleteDraft(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false)
{
	HTTPPacket::HTTPResponsePacket response;
	response.SetServer(SERVER_SIGNATURE);

	if (request.method != "POST")
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::MethodNotAllowed);
		return response;
	}

	std::map<std::string, std::string> resultJson;
	std::string draftId = webstring::ParseKeyValue(request.body)["draft_id"];
	if (draftId == "")
	{
		resultJson["ecode"] = "-5";
		resultJson["reason"] = "请求参数有误";
		response.body = webstring::JsonStringify(resultJson);

		response.SetContentType("application/json; charset=UTF-8");
		return response;
	}

	try
	{
		if (CheckUserToken(request) == false)
		{
			response.SetResponseCode(HTTPPacket::ResponseCode::Forbidden);
			return response;
		}

		mysqlProperty.connection->setSchema("lxhblogspace_content");
		PtrPreparedStatement statement(mysqlProperty.connection->prepareStatement("SELECT * FROM draft_info WHERE user_uuid=? AND draft_id=?"));
		statement->setString(1, request.GetCookieValue("_uuid"));
		statement->setString(2, draftId);
		PtrResultSet result(statement->executeQuery());
		while (result->next())
		{
			PtrPreparedStatement deleteStatement(mysqlProperty.connection->prepareStatement("UPDATE draft_info SET deleted=1 WHERE draft_id=?"));
			deleteStatement->setString(1, draftId);
			deleteStatement->executeUpdate();
			
			resultJson["ecode"] = "0";
			resultJson["reason"] = "操作成功";
			response.body = webstring::JsonStringify(resultJson);

			response.SetContentType("application/json; charset=UTF-8");
			return response;
		}

		//执行至此说明指定草稿不存在
		resultJson["ecode"] = "-4";
		resultJson["reason"] = "指定草稿不存在";
		response.body = webstring::JsonStringify(resultJson);

		response.SetContentType("application/json; charset=UTF-8");
		return response;

	}
	catch (sql::SQLException e)
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::ServiceUnavailable);
		if (serverProperty.verbose >= VerboseLevel::essential)
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::DeleteDraft(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()) + ", msg: " + e.what());
		}
		else
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::DeleteDraft(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()));
		}

		return response;
	}

}

HTTPPacket::HTTPResponsePacket BlogSpaceContent::GetDraftList(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false)
{
	HTTPPacket::HTTPResponsePacket response;
	response.SetServer(SERVER_SIGNATURE);

	if (request.method != "GET")
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::MethodNotAllowed);
		return response;
	}
	
	nlohmann::json draftList;

	try
	{
		if (CheckUserToken(request) == false)
		{
			response.SetResponseCode(HTTPPacket::ResponseCode::Forbidden);
			return response;
		}

		response.SetContentType("application/json; charset=UTF-8");

		std::string uuid = request.GetCookieValue("_uuid");

		mysqlProperty.connection->setSchema("lxhblogspace_content");
		PtrPreparedStatement statement(mysqlProperty.connection->prepareStatement("SELECT draft_id, lastmodify_date, deleted FROM draft_info WHERE user_uuid=? AND deleted=0 ORDER BY lastmodify_date DESC"));
		statement->setString(1, uuid);
		PtrResultSet result(statement->executeQuery());
		if (result->rowsCount() == 0)
		{
			response.body = "[]";
			return response;
		}

		while (result->next())
		{
			nlohmann::json draft;
			draft["draft_id"] = result->getString(1);
			draft["lastmodify_date"] = result->getUInt(2);

			PtrPreparedStatement draftContentST(mysqlProperty.connection->prepareStatement("SELECT title, content FROM draft_content WHERE draft_id=?"));
			draftContentST->setString(1, draft["draft_id"].get<std::string>());
			PtrResultSet draftContent(draftContentST->executeQuery());
			while (draftContent->next())
			{
				draft["title"] = draftContent->getString(1);
				draft["content"] = draftContent->getString(2);
				if (draft["content"].get<std::string>().length() > 100)
				{
					draft["content"] = draft["content"].get<std::string>().substr(0, 100) + "...";
				}
			}
			draftList.push_back(draft);
		}

	}
	catch (sql::SQLException e)
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::ServiceUnavailable);
		if (serverProperty.verbose >= VerboseLevel::essential)
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::GetDraftList(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()) + ", msg: " + e.what());
		}
		else
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::GetDraftList(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()));
		}

		return response;
	}

	
	response.body = draftList.dump();

	return response;
}

HTTPPacket::HTTPResponsePacket BlogSpaceContent::PublishArticle(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false)
{
	HTTPPacket::HTTPResponsePacket response;
	response.SetServer(SERVER_SIGNATURE);

	if (request.method != "POST")
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::MethodNotAllowed);
		return response;
	}

	std::map<std::string, std::string> responseJson;

	try
	{
		if (CheckUserToken(request) == false)
		{
			response.SetResponseCode(HTTPPacket::ResponseCode::Forbidden);
			return response;
		}
	}
	catch (sql::SQLException e)
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::ServiceUnavailable);
		if (serverProperty.verbose >= VerboseLevel::essential)
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::PublishArticle(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()) + ", msg: " + e.what());
		}
		else
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::PublishArticle(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()));
		}

		return response;
	}

	auto postParamaters = webstring::ParseKeyValue(request.body);
	if (postParamaters.count("bg") == 0 || postParamaters.count("title") == 0 || postParamaters.count("content") == 0)
	{
		responseJson.insert({ "ecode", "-2" });
		responseJson.insert({ "reason", "请求参数有误" });

		response.SetContentType("application/json; charset=UTF-8");
		response.body = webstring::JsonStringify(responseJson);
		return response;
	}

	std::string articleId = postParamaters["article_id"];
	std::string backgroundPath = webstring::URLdecode(postParamaters["bg"]);
	std::string articleTitle = postParamaters["title"];
	std::string articleContent = postParamaters["content"];

	if (articleTitle.length() > 0)
	{
		articleTitle = webstring::Base64Decode(articleTitle);
	}

	if (articleContent.length() > 0)
	{
		articleContent = webstring::Base64Decode(articleContent);
	}

	if (webstring::UTF8Strlen(articleTitle) > 50 || webstring::UTF8Strlen(articleContent) > 50000)
	{
		responseJson.insert({ "ecode", "-3" });
		responseJson.insert({ "reason", "文章标题或内容过长" });

		response.SetContentType("application/json; charset=UTF-8");
		response.body = webstring::JsonStringify(responseJson);
		return response;
	}

	try
	{
		//如果article_id不为空，则更新文章
		if (articleId != "")
		{
			mysqlProperty.connection->setSchema("lxhblogspace_content");
			PtrPreparedStatement statement(mysqlProperty.connection->prepareStatement("SELECT article_id, deleted FROM article_info WHERE user_uuid=?"));
			statement->setString(1, request.GetCookieValue("_uuid"));
			PtrResultSet result(statement->executeQuery());
			while (result->next())
			{
				if (result->getString(1) == articleId)
				{
					if (result->getUInt("deleted") == 1)
					{
						break;
					}

					PtrStatement transaction(mysqlProperty.connection->createStatement());
					transaction->execute("BEGIN");

					statement.reset(mysqlProperty.connection->prepareStatement("UPDATE article_info SET lastmodify_date=?, background_img=? WHERE article_id=?"));
					statement->setString(1, webstring::GenTimeStamp());
					statement->setString(2, backgroundPath);
					statement->setString(3, articleId);
					statement->executeUpdate();

					statement.reset(mysqlProperty.connection->prepareStatement("UPDATE article_content SET title=?, content=? WHERE article_id=?"));
					statement->setString(1, articleTitle);
					statement->setString(2, articleContent);
					statement->setString(3, articleId);
					statement->executeUpdate();

					transaction->execute("COMMIT");

					responseJson.insert({ "ecode", "0" });
					responseJson.insert({ "reason", "保存成功" });
					responseJson.insert({ "article_id", articleId });

					response.SetContentType("application/json; charset=UTF-8");
					response.body = webstring::JsonStringify(responseJson);
					return response;
				}
			}

			responseJson.insert({ "ecode", "-4" });
			responseJson.insert({ "reason", "指定文章在你的文章列表中不存在" });
			responseJson.insert({ "article_id", articleId });

			response.SetContentType("application/json; charset=UTF-8");
			response.body = webstring::JsonStringify(responseJson);
			return response;
		}

		PtrStatement transaction(mysqlProperty.connection->createStatement());
		transaction->execute("BEGIN");

		mysqlProperty.connection->setSchema("lxhblogspace_content");
		PtrPreparedStatement statement(mysqlProperty.connection->prepareStatement("INSERT INTO article_info(user_uuid, create_date, lastmodify_date, background_img, deleted, vote_num) VALUES(?,?,?,?,?,?)"));

		statement->setString(1, request.GetCookieValue("_uuid"));
		statement->setString(2, webstring::GenTimeStamp());
		statement->setString(3, webstring::GenTimeStamp());
		statement->setString(4, backgroundPath);
		statement->setUInt(5, 0);
		statement->setUInt(6, 0);
		statement->executeUpdate();

		std::string insertID;
		statement.reset(mysqlProperty.connection->prepareStatement("SELECT LAST_INSERT_ID()"));
		PtrResultSet result(statement->executeQuery());
		while (result->next())
		{
			insertID = result->getString(1);
		}

		statement.reset(mysqlProperty.connection->prepareStatement("INSERT INTO article_content VALUES(?,?,?)"));
		statement->setString(1, insertID);
		statement->setString(2, articleTitle);
		statement->setString(3, articleContent);
		statement->executeUpdate();

		transaction->execute("COMMIT");

		responseJson.insert({ "ecode", "0" });
		responseJson.insert({ "reason", "发布成功" });
		responseJson.insert({ "article_id", insertID });

		response.SetContentType("application/json; charset=UTF-8");
		response.body = webstring::JsonStringify(responseJson);
	}
	catch (sql::SQLException e)
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::ServiceUnavailable);
		if (serverProperty.verbose >= VerboseLevel::essential)
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::PublishArticle(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()) + ", msg: " + e.what());
		}
		else
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::PublishArticle(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()));
		}

		return response;
	}


	return response;
}

HTTPPacket::HTTPResponsePacket BlogSpaceContent::GetArticleContent(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false)
{
	HTTPPacket::HTTPResponsePacket response;
	response.SetServer(SERVER_SIGNATURE);

	if (request.method != "GET")
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::MethodNotAllowed);
		return response;
	}

	std::map<std::string, std::string> responseJson;


	auto postParamaters = request.ParseURLParamter();
	if (postParamaters["article_id"] == "")
	{
		responseJson["ecode"] = "-5";
		responseJson["reason"] = "请求参数有误";

		response.SetContentType("application/json; charset=UTF-8");
		response.body = webstring::JsonStringify(responseJson);
		return response;
	}

	std::string articleId = postParamaters["article_id"];
	std::string user_uuid = request.GetCookieValue("_uuid");

	try
	{
		mysqlProperty.connection->setSchema("lxhblogspace_content");
		PtrPreparedStatement statement(mysqlProperty.connection->prepareStatement("SELECT user_uuid, lastmodify_date, background_img, deleted, vote_num FROM article_info WHERE article_id=?"));
		statement->setString(1, articleId);
		PtrResultSet result(statement->executeQuery());
		while (result->next())
		{
			if (result->getUInt("deleted") == 1)
			{
				break;
			}

			std::string author_uuid = result->getString(1);
			responseJson["article_id"] = articleId;
			responseJson["lastmodify_time"] = result->getString(2);
			responseJson["bg"] = webstring::URLencode(result->getString(3));
			responseJson["vote_num"] = result->getString(5);
			responseJson["comment_num"] = std::to_string(GetArticleCommentNum(articleId));

			statement.reset(mysqlProperty.connection->prepareStatement("SELECT title, content FROM article_content WHERE article_id=?"));
			statement->setString(1, articleId);
			PtrResultSet article(statement->executeQuery());
			while (article->next())
			{
				responseJson["title"] = webstring::Base64Encode(article->getString(1));
				responseJson["content"] = webstring::Base64Encode(article->getString(2));
			}

			//获取作者的用户名
			mysqlProperty.connection->setSchema("lxhblogspace_passport");
			statement.reset(mysqlProperty.connection->prepareStatement("SELECT username FROM user_info WHERE user_uuid=?"));
			statement->setString(1, author_uuid);
			PtrResultSet authorInfo(statement->executeQuery());
			while (authorInfo->next())
			{
				responseJson["author"] = authorInfo->getString(1);
			}

			responseJson.insert({ "ecode", "0" });

			response.SetContentType("application/json; charset=UTF-8");
			response.body = webstring::JsonStringify(responseJson);
			return response;
		}

		//执行至此说明指定的articleId不存在
		responseJson["ecode"] = "-4";
		responseJson["reason"] = "指定文章不存在";

		response.SetContentType("application/json; charset=UTF-8");
		response.body = webstring::JsonStringify(responseJson);
		return response;

	}
	catch (sql::SQLException e)
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::ServiceUnavailable);
		if (serverProperty.verbose >= VerboseLevel::essential)
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::GetArticleContent(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()) + ", msg: " + e.what());
		}
		else
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::GetArticleContent(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()));
		}

		return response;
	}
}

HTTPPacket::HTTPResponsePacket BlogSpaceContent::DeleteArticle(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false)
{
	HTTPPacket::HTTPResponsePacket response;
	response.SetServer(SERVER_SIGNATURE);

	if (request.method != "POST")
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::MethodNotAllowed);
		return response;
	}

	auto requestParam = webstring::ParseKeyValue(request.body);
	std::map<std::string, std::string> resultJson;

	if (requestParam.count("article_id") == 0)
	{
		resultJson["ecode"] = "-5";
		resultJson["reason"] = "请求参数有误";

		response.body = webstring::JsonStringify(resultJson);
		return response;
	}

	std::string articleId = requestParam["article_id"];
	try
	{
		if (CheckUserToken(request) == false)
		{
			response.SetResponseCode(HTTPPacket::ResponseCode::Forbidden);
			return response;
		}

		mysqlProperty.connection->setSchema("lxhblogspace_content");
		PtrPreparedStatement statement(mysqlProperty.connection->prepareStatement("SELECT user_uuid FROM article_info WHERE article_id=?"));
		statement->setString(1, articleId);
		PtrResultSet result(statement->executeQuery());
		while (result->next())
		{
			if (result->getString("user_uuid") != request.GetCookieValue("_uuid"))
			{
				response.SetResponseCode(HTTPPacket::ResponseCode::Forbidden);
				return response;
			}

			statement.reset(mysqlProperty.connection->prepareStatement("UPDATE article_info SET deleted=1 WHERE article_id=?"));
			statement->setString(1, articleId);
			statement->executeUpdate();

			mysqlProperty.connection->setSchema("lxhblogspace_member");
			statement.reset(mysqlProperty.connection->prepareStatement("INSERT INTO sys_notice(recipient_uuid, msg_title, msg, create_date, readed) VALUES(?,?,?,?,?)"));
			statement->setString(1, GetArticleAuthor(articleId));
			statement->setString(2, std::string("你的文章：") + GetArticleTitle(articleId) + " 已被删除");
			statement->setString(3, "原因：用户手动删除");
			statement->setString(4, webstring::GenTimeStamp());
			statement->setUInt(5, 0);
			statement->execute();

			resultJson["ecode"] = "0";
			resultJson["reason"] = "删除成功";
			response.SetContentType("application/json; charset=UTF-8");
			response.body = webstring::JsonStringify(resultJson);
			return response;
		}

		resultJson["ecode"] = "-4";
		resultJson["reason"] = "指定文章不存在";

		response.SetContentType("application/json; charset=UTF-8");
		response.body = webstring::JsonStringify(resultJson);
		return response;
	}
	catch (sql::SQLException e)
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::ServiceUnavailable);
		if (serverProperty.verbose >= VerboseLevel::essential)
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::DeleteArticle(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()) + ", msg: " + e.what());
		}
		else
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::DeleteArticle(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()));
		}

		return response;
	}
}


HTTPPacket::HTTPResponsePacket BlogSpaceContent::GetUserArticleList(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false)
{
	HTTPPacket::HTTPResponsePacket response;
	response.SetServer(SERVER_SIGNATURE);
	response.SetContentType("application/json; charset=UTF-8");

	if (request.method != "GET")
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::MethodNotAllowed);
		return response;
	}

	std::string username = request.ParseURLParamter()["username"];
	std::vector<std::string> articleInfoList;
	std::map<std::string, std::string> articleInfo;

	if (username == "")
	{
		response.body = "[]";
		return response;
	}

	try
	{
		std::string user_uuid = GetUUIDByUsername(username);
		if (user_uuid == "")
		{
			response.body = "[]";
			return response;
		}
		mysqlProperty.connection->setSchema("lxhblogspace_content");
		PtrPreparedStatement statement(mysqlProperty.connection->prepareStatement("SELECT article_id, create_date, vote_num FROM article_info WHERE user_uuid=? AND deleted=0 ORDER BY article_id DESC"));
		statement->setString(1, user_uuid);
		PtrResultSet article_info_rows(statement->executeQuery());
		while (article_info_rows->next())
		{
			articleInfo["article_id"] = article_info_rows->getString(1);
			articleInfo["create_date"] = article_info_rows->getString(2);
			articleInfo["vote_num"] = article_info_rows->getString(3);
			articleInfo["author"] = username;
			PtrPreparedStatement st(mysqlProperty.connection->prepareStatement("SELECT title, content FROM article_content WHERE article_id=?"));
			st->setString(1, articleInfo["article_id"]);
			PtrResultSet article_content_rows(st->executeQuery());
			while (article_content_rows->next())
			{
				articleInfo["title"] = webstring::Base64Encode(article_content_rows->getString(1));
				articleInfo["brief"] = MarkdownBriefPretty(article_content_rows->getString(2));
				if (webstring::UTF8Strlen(articleInfo["brief"]) > 80)
				{
					articleInfo["brief"] = webstring::UTF8Substr(articleInfo["brief"], 0, 80) + "...";
				}
				articleInfo["brief"] = webstring::Base64Encode(articleInfo["brief"]);
			}

			st.reset(mysqlProperty.connection->prepareStatement("SELECT COUNT(*) FROM article_comment WHERE article_id=?"));
			st->setString(1, articleInfo["article_id"]);
			PtrResultSet article_comment_rows(st->executeQuery());
			article_comment_rows->next();
			articleInfo["comment_num"] = article_comment_rows->getString(1);


			articleInfoList.push_back(webstring::JsonStringify(articleInfo));
		}
	}
	catch (sql::SQLException e)
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::ServiceUnavailable);
		if (serverProperty.verbose >= VerboseLevel::essential)
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::GetUserArticleList(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()) + ", msg: " + e.what());
		}
		else
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::GetUserArticleList(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()));
		}
		return response;
	}

	std::string result = "[";
	for (auto article : articleInfoList)
	{
		result += article + ", ";
	}
	result = result.substr(0, result.length() - 2);
	result += "]";

	response.body = result;
	return response;
}

HTTPPacket::HTTPResponsePacket BlogSpaceContent::Vote(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false)
{
	HTTPPacket::HTTPResponsePacket response;
	response.SetServer(SERVER_SIGNATURE);
	

	if (request.method != "POST")
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::MethodNotAllowed);
		return response;
	}

	std::map<std::string, std::string> resultJson;
	try
	{
		if (CheckUserToken(request) == false)
		{
			response.SetResponseCode(HTTPPacket::ResponseCode::Forbidden);
			return response;
		}

		response.SetContentType("application/json; charset=UTF-8");
		std::string user_uuid = request.GetCookieValue("_uuid");
		auto requestParam = webstring::ParseKeyValue(request.body);
		if ((requestParam["action"] != "cancle" && requestParam["action"] != "vote") || requestParam["article_id"] == "")
		{
			resultJson["ecode"] = "-5";
			resultJson["reason"] = "请求参数有误";
			response.body = webstring::JsonStringify(resultJson);

			return response;
		}

		std::string articleId = requestParam["article_id"];
		if (IsArticleExist(articleId) == false)
		{
			resultJson["ecode"] = "-4";
			resultJson["reason"] = "指定文章不存在";
			response.body = webstring::JsonStringify(resultJson);

			return response;
		}

		if (requestParam["action"] == "cancle")
		{
			if (IsVoted(user_uuid, articleId) == true)
			{
				mysqlProperty.connection->setSchema("lxhblogspace_content");
				PtrPreparedStatement statement(mysqlProperty.connection->prepareStatement("DELETE FROM article_vote_history WHERE user_uuid=? AND article_id=?"));
				statement->setString(1, user_uuid);
				statement->setString(2, articleId);
				statement->executeUpdate();
			}
		}
		else
		{
			if (IsVoted(user_uuid, articleId) == false)
			{
				mysqlProperty.connection->setSchema("lxhblogspace_content");
				PtrPreparedStatement statement(mysqlProperty.connection->prepareStatement("INSERT INTO article_vote_history VALUES(?,?,?)"));
				statement->setString(1, articleId);
				statement->setString(2, user_uuid);
				statement->setString(3, webstring::GenTimeStamp());
				statement->executeUpdate();
			}
		}

		PtrPreparedStatement updateStatement(mysqlProperty.connection->prepareStatement("UPDATE article_info a JOIN (SELECT COUNT(*) AS nums FROM article_vote_history WHERE article_id=?) b SET a.vote_num=b.nums WHERE a.article_id=?;"));
		updateStatement->setString(1, articleId);
		updateStatement->setString(2, articleId);
		updateStatement->execute();


	}
	catch (sql::SQLException e)
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::ServiceUnavailable);
		if (serverProperty.verbose >= VerboseLevel::essential)
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::Vote(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()) + ", msg: " + e.what());
		}
		else
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::Vote(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()));
		}
		return response;
	}

	resultJson["ecode"] = "0";
	resultJson["reason"] = "操作成功";
	response.body = webstring::JsonStringify(resultJson);
	return response;
	
}

HTTPPacket::HTTPResponsePacket BlogSpaceContent::Subscribe(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false)
{
	HTTPPacket::HTTPResponsePacket response;
	response.SetServer(SERVER_SIGNATURE);


	if (request.method != "POST")
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::MethodNotAllowed);
		return response;
	}

	std::map<std::string, std::string> resultJson;
	try
	{
		if (CheckUserToken(request) == false)
		{
			response.SetResponseCode(HTTPPacket::ResponseCode::Forbidden);
			return response;
		}

		response.SetContentType("application/json; charset=UTF-8");

		std::string user_uuid = request.GetCookieValue("_uuid");
		auto requestParam = webstring::ParseKeyValue(request.body);
		if (requestParam["article_id"] == "")
		{
			resultJson["ecode"] = "-5";
			resultJson["reason"] = "请求参数有误";
			response.body = webstring::JsonStringify(resultJson);

			return response;
		}
		std::string articleId = requestParam["article_id"];

		if (IsArticleExist(articleId) == false)
		{
			resultJson["ecode"] = "-4";
			resultJson["reason"] = "指定文章不存在";
			response.body = webstring::JsonStringify(resultJson);

			return response;
		}

		if (requestParam["action"] == "cancle")
		{
			if (IsSubscribed(user_uuid, articleId) == true)
			{
				mysqlProperty.connection->setSchema("lxhblogspace_content");
				PtrPreparedStatement statement(mysqlProperty.connection->prepareStatement("DELETE FROM article_subscribe_history WHERE user_uuid=? AND article_id=?"));
				statement->setString(1, user_uuid);
				statement->setString(2, articleId);
				statement->executeUpdate();
			}
		}
		else if (requestParam["action"] == "subscribe")
		{
			if (IsSubscribed(user_uuid, articleId) == false)
			{
				mysqlProperty.connection->setSchema("lxhblogspace_content");
				PtrPreparedStatement statement(mysqlProperty.connection->prepareStatement("INSERT INTO article_subscribe_history VALUES(?,?,?)"));
				statement->setString(1, articleId);
				statement->setString(2, user_uuid);
				statement->setString(3, webstring::GenTimeStamp());
				statement->executeUpdate();
			}
		}
		else
		{
			resultJson["ecode"] = "-5";
			resultJson["reason"] = "请求参数有误";
			response.body = webstring::JsonStringify(resultJson);

			return response;
		}

	}
	catch (sql::SQLException e)
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::ServiceUnavailable);
		if (serverProperty.verbose >= VerboseLevel::essential)
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::Subscribe(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()) + ", msg: " + e.what());
		}
		else
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::Subscribe(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()));
		}
		return response;
	}

	resultJson["ecode"] = "0";
	resultJson["reason"] = "操作成功";
	response.body = webstring::JsonStringify(resultJson);
	return response;
}

HTTPPacket::HTTPResponsePacket BlogSpaceContent::GetSubscribedList(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false)
{
	HTTPPacket::HTTPResponsePacket response;
	response.SetServer(SERVER_SIGNATURE);

	nlohmann::json responseJson;
	
	try
	{
		if (CheckUserToken(request) == false)
		{
			response.SetResponseCode(HTTPPacket::ResponseCode::Forbidden);
			return response;
		}

		mysqlProperty.connection->setSchema("lxhblogspace_content");
		PtrPreparedStatement statement(mysqlProperty.connection->prepareStatement("SELECT ta.article_id, ta.action_date, tb.title, tc.user_uuid AS author_uuid FROM article_subscribe_history ta JOIN article_content tb ON ta.article_id=tb.article_id AND ta.user_uuid=? JOIN lxhblogspace_content.article_info tc ON  ta.article_id=tc.article_id ORDER BY ta.action_date DESC"));
		statement->setString(1, request.GetCookieValue("_uuid"));
		response.body = "[]";
		response.SetContentType("application/x-www-form/urlencoded");

		PtrResultSet result(statement->executeQuery());
		while (result->next())
		{
			responseJson.push_back
			({
				{"article_id", result->getUInt("article_id")},
				{"title", result->getString("title")},
				{"author", GetUsernameByUUID(result->getString("author_uuid"))},
				{"avatar", GetUserAvatar(result->getString("author_uuid"))},
				{"add_time", result->getUInt("action_date")}
			});
		}

		if (responseJson.size() != 0)
		{
			response.body = responseJson.dump();
		}

		return response;
	}
	catch (sql::SQLException e)
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::ServiceUnavailable);
		if (serverProperty.verbose >= VerboseLevel::essential)
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::GetSubscribedList(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()) + ", msg: " + e.what());
		}
		else
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::GetSubscribedList(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()));
		}
		return response;
	}
}

HTTPPacket::HTTPResponsePacket BlogSpaceContent::CheckUserOperation(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false)
{
	HTTPPacket::HTTPResponsePacket response;
	response.SetServer(SERVER_SIGNATURE);

	if (request.method != "GET")
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::MethodNotAllowed);
		return response;
	}

	response.SetContentType("application/json; charset=UTF-8");

	std::map<std::string, std::string> resultJson;
	std::string articleId = request.ParseURLParamter()["article_id"];
	if (articleId == "")
	{
		resultJson["ecode"] = "-4";
		resultJson["reason"] = "指定文章不存在";
		response.body = webstring::JsonStringify(resultJson);

		return response;
	}

	try
	{
		if (CheckUserToken(request) == false)
		{
			
			resultJson["ecode"] = "0";
			resultJson["isAuthor"] = "0";
			resultJson["can_vote"] = "1";
			resultJson["can_subscribe"] = "1";

			response.body = webstring::JsonStringify(resultJson);
			return response;
		}

		std::string user_uuid = request.GetCookieValue("_uuid");
		mysqlProperty.connection->setSchema("lxhblogspace_content");
		PtrPreparedStatement statement(mysqlProperty.connection->prepareStatement("SELECT user_uuid FROM article_info WHERE article_id=?"));
		statement->setString(1, articleId);
		PtrResultSet result(statement->executeQuery());

		if (result->rowsCount() == 0)
		{
			resultJson["ecode"] = "-4";
			resultJson["reason"] = "指定文章不存在";
			response.body = webstring::JsonStringify(resultJson);
		}

		resultJson["ecode"] = "0";
		while (result->next())
		{
			if (result->getString(1) == user_uuid)
			{
				resultJson["isAuthor"] = "1";
			}
			else
			{
				resultJson["isAuthor"] = "0";
			}
		}

		if (IsVoted(user_uuid, articleId) == true)
		{
			resultJson["can_vote"] = "0";
		}
		else
		{
			resultJson["can_vote"] = "1";
		}

		if (IsSubscribed(user_uuid, articleId) == true)
		{
			resultJson["can_subscribe"] = "0";
		}
		else
		{
			resultJson["can_subscribe"] = "1";
		}
	}
	catch (sql::SQLException e)
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::ServiceUnavailable);
		if (serverProperty.verbose >= VerboseLevel::essential)
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::CheckUserOperation(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()) + ", msg: " + e.what());
		}
		else
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::CheckUserOperation(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()));
		}
		return response;
	}

	response.body = webstring::JsonStringify(resultJson);
	return response;
}

HTTPPacket::HTTPResponsePacket BlogSpaceContent::SendComment(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false)
{
	HTTPPacket::HTTPResponsePacket response;
	response.SetServer(SERVER_SIGNATURE);
	
	if (request.method != "POST")
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::MethodNotAllowed);
		return response;
	}

	std::map<std::string, std::string> resultJson;
	try
	{
		if (CheckUserToken(request) == false)
		{
			response.SetResponseCode(HTTPPacket::ResponseCode::Forbidden);
			return response;
		}

	
		auto requestParam = webstring::ParseKeyValue(request.body);
		std::string articleId = requestParam["article_id"];
		std::string content = requestParam["content"];

		if (articleId == "" || content == "")
		{
			resultJson["ecode"] = "-5";
			resultJson["reason"] = "请求参数有误";

			response.SetContentType("application/json; charset=UTF-8");
			response.body = webstring::JsonStringify(resultJson);
			return response;
		}

		if (IsArticleExist(articleId) == false)
		{
			resultJson["ecode"] = "-4";
			resultJson["reason"] = "指定文章不存在";

			response.SetContentType("application/json; charset=UTF-8");
			response.body = webstring::JsonStringify(resultJson);
			return response;
		}

		mysqlProperty.connection->setSchema("lxhblogspace_content");
		PtrPreparedStatement statement(mysqlProperty.connection->prepareStatement("INSERT INTO article_comment(article_id, from_uuid, to_uuid, create_date, content) VALUES(?,?,?,?,?)"));
		statement->setString(1, articleId);
		statement->setString(2, request.GetCookieValue("_uuid"));
		statement->setString(3, "");
		statement->setString(4, webstring::GenTimeStamp());
		statement->setString(5, webstring::Base64Decode(content));
		statement->execute();

		statement.reset(mysqlProperty.connection->prepareStatement("SET @lastInserid=LAST_INSERT_ID()"));
		statement->execute();

		mysqlProperty.connection->setSchema("lxhblogspace_member");
		PtrPreparedStatement pushst(mysqlProperty.connection->prepareStatement("INSERT INTO user_comment_notice(recipient_uuid, comment_id, readed, deleted) VALUES(?,@lastInserid,0,0)"));
		pushst->setString(1, GetArticleAuthor(articleId));
		pushst->execute();

		resultJson["ecode"] = "0";

		response.SetContentType("application/json; charset=UTF-8");
		response.body = webstring::JsonStringify(resultJson);


		return response;
	}
	catch (sql::SQLException e)
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::ServiceUnavailable);
		if (serverProperty.verbose >= VerboseLevel::essential)
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::SendComment(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()) + ", msg: " + e.what());
		}
		else
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::SendComment(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()));
		}
		return response;
	}
}

HTTPPacket::HTTPResponsePacket BlogSpaceContent::GetComments(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false)
{
	HTTPPacket::HTTPResponsePacket response;
	using boost::property_tree::ptree;
	using boost::property_tree::write_json;

	if (request.method != "GET")
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::MethodNotAllowed);
		return response;
	}

	std::vector<nlohmann::json> commentList;
	nlohmann::json commentInfo = { {"pageNum", -1}, {"maxPageNum", -1}, {"commentList", commentList} };

	auto requestParam = request.ParseURLParamter();
	std::string articleId = requestParam["article_id"];
	int requestedPage;
	try
	{
		if (IsArticleExist(articleId) == false)
		{
			response.SetContentType("application/json; charset=UTF-8");
			response.body = commentInfo.dump();
			return response;
		}

		mysqlProperty.connection->setSchema("lxhblogspace_content");
		PtrPreparedStatement statement(mysqlProperty.connection->prepareStatement("SELECT COUNT(*) FROM article_comment WHERE article_id=?"));
		statement->setString(1, articleId);
		PtrResultSet result(statement->executeQuery());
		result->next();
		int commentAmount = result->getInt(1);

		try
		{
			requestedPage = std::stoi(requestParam["page"]);
		}
		catch (...)
		{
			response.SetContentType("application/json; charset=UTF-8");
			response.body = commentInfo.dump();
			return response;
		}

		if (requestedPage <= 0 || ((requestedPage - 1) * 5 > commentAmount))
		{
			response.SetContentType("application/json; charset=UTF-8");
			response.body = commentInfo.dump();
			return response;
		}

		mysqlProperty.connection->setSchema("lxhblogspace_content");
		PtrPreparedStatement commentContentst(mysqlProperty.connection->prepareStatement("SELECT from_uuid, to_uuid, create_date, content FROM article_comment WHERE article_id=? ORDER BY comment_id LIMIT ?,5"));
		commentContentst->setString(1, articleId);
		commentContentst->setInt(2, (requestedPage - 1) * 5);
		PtrResultSet commentContent(commentContentst->executeQuery());
		while (commentContent->next())
		{
			commentList.push_back
			({
				{ "username", GetUsernameByUUID(commentContent->getString(1)) },
				{ "avatar", GetUserAvatar(commentContent->getString(1)) },
				{ "re", GetUsernameByUUID(commentContent->getString(2)) },
				{ "time", commentContent->getString(3) },
				{ "comment", commentContent->getString(4) }
			});
		}

		commentInfo["commentList"] = commentList;
		commentInfo["pageNum"] = requestedPage;
		int maxPageNum = commentAmount / 5;
		if (commentAmount % 5)
		{
			++maxPageNum;
		}
		//除运算符返回的结果类型取决于被除数的数据类型。
		commentInfo["maxPageNum"] = maxPageNum;

		response.SetContentType("application/json; charset=UTF-8");
		response.body = commentInfo.dump();

		return response;
	}
	catch (sql::SQLException e)
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::ServiceUnavailable);
		if (serverProperty.verbose >= VerboseLevel::essential)
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::GetComments(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()) + ", msg: " + e.what());
		}
		else
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::GetComments(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()));
		}
		return response;
	}
	
}

HTTPPacket::HTTPResponsePacket BlogSpaceContent::GetPublishArticleList(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false)
{
	using boost::property_tree::ptree;
	using boost::property_tree::write_json;

	HTTPPacket::HTTPResponsePacket response;
	response.SetServer(SERVER_SIGNATURE);

	if (request.method != "GET")
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::MethodNotAllowed);
		return response;
	}

	std::string lastArticleId = request.ParseURLParamter()["article_id"];
	int lastNode;
	std::vector<std::string> articleList;

	try
	{
		mysqlProperty.connection->setSchema("lxhblogspace_content");
		try
		{
			lastNode = std::stoi(lastArticleId);
			if (lastNode <= 0)
			{
				throw std::runtime_error("");
			}
		}
		catch (...)
		{
			lastNode = -1;
		}

		PtrPreparedStatement statement;
		if (lastNode > 0)
		{
			statement.reset(mysqlProperty.connection->prepareStatement("SELECT article_id, user_uuid, create_date, vote_num FROM article_info WHERE article_id < ? AND deleted=0 ORDER BY article_id DESC LIMIT 10"));
			statement->setInt(1, lastNode);
		}
		else
		{
			statement.reset(mysqlProperty.connection->prepareStatement("SELECT article_id, user_uuid, create_date, vote_num FROM article_info WHERE deleted=0 ORDER BY article_id DESC LIMIT 10"));
		}
		
		PtrResultSet result(statement->executeQuery());
		if (result->rowsCount() == 0)
		{
			response.SetContentType("applicatoin/json; charset=UTF-8");

			response.body = "[]";
			return response;
		}

		while (result->next())
		{
			std::string articleId = result->getString(1);
			std::string user_uuid = result->getString(2);

			time_t create_date = result->getUInt(3);
			int vote_num = result->getInt(4);
			int comment_num = GetArticleCommentNum(articleId);

			std::string username = GetUsernameByUUID(user_uuid);
			std::string avatar = GetUserAvatar(user_uuid);

			PtrPreparedStatement articleContentst(mysqlProperty.connection->prepareStatement("SELECT title, content FROM article_content WHERE article_id=?"));
			articleContentst->setString(1, articleId);
			PtrResultSet articleContent(articleContentst->executeQuery());
			articleContent->next();

			std::string title = articleContent->getString(1);
			std::string content = MarkdownBriefPretty(articleContent->getString(2));
			if (webstring::UTF8Strlen(content) > 100)
			{
				content = webstring::UTF8Substr(content, 0, 100) + "...";
			}

			ptree articleInfoTreeRoot;
			articleInfoTreeRoot.put("article_id", articleId);
			articleInfoTreeRoot.put("title", webstring::Base64Encode(title));
			articleInfoTreeRoot.put("brief", webstring::Base64Encode(content));
			articleInfoTreeRoot.put("create_date", create_date);

			ptree interInfo;
			interInfo.put("vote", vote_num);
			interInfo.put("comments", comment_num);

			articleInfoTreeRoot.add_child("interInfo", interInfo);

			ptree authorInfo;
			authorInfo.put("name", username);
			authorInfo.put("avatar", avatar);

			articleInfoTreeRoot.add_child("authorInfo", authorInfo);

			ptree tagInfo;
			ptree tag1, tag2;

			tag1.put("", "文章标签1");
			tag2.put("", "文章标签2");
			tagInfo.push_back(std::make_pair("", tag1));
			tagInfo.push_back(std::make_pair("", tag2));

			articleInfoTreeRoot.add_child("tags", tagInfo);

			std::stringstream stream;
			write_json(stream, articleInfoTreeRoot, false);
			articleList.push_back(stream.str());
		}

		std::string articleListStr = "[";
		for (auto article : articleList)
		{
			articleListStr += article + ", ";
		}

		articleListStr = articleListStr.substr(0, articleListStr.length() - 2);
		articleListStr += "]";

		response.SetContentType("application/json; charset=UTF-8");
		response.body = articleListStr;

		return response;

	}
	catch (sql::SQLException& e)
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::ServiceUnavailable);
		if (serverProperty.verbose >= VerboseLevel::essential)
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::GetComments(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()) + ", msg: " + e.what());
		}
		else
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::GetComments(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()));
		}
		return response;
	}
}

HTTPPacket::HTTPResponsePacket BlogSpaceContent::GetFollowedArticleList(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false)
{
	HTTPPacket::HTTPResponsePacket response;

	nlohmann::json responseJson;
	auto requestParam = request.ParseURLParamter();


	try
	{
		if (CheckUserToken(request) == false)
		{
			response.SetResponseCode(HTTPPacket::ResponseCode::Forbidden);
			return response;
		}

		PtrPreparedStatement statement;
		int index = 0;
		long time = 0;

		if (requestParam.count("time") != 0)
		{
			
			try
			{
				time = std::stol(requestParam["time"]);
				index = std::stoi(requestParam["index"]);
				if (time < 0 || index < 0)
				{
					throw std::invalid_argument("...");
				}
			}
			catch (...)
			{
				responseJson["ecode"] = -5;
				responseJson["reason"] = "传递的参数无效";

				response.body = responseJson.dump();
				response.SetContentType("application/json; charset=utf8");

				return response;
			}

			statement.reset(mysqlProperty.connection->prepareStatement(
				"SELECT tb.article_id, tc.title, tb.create_date, tb.vote_num, tc.content, td.username, te.avatar FROM lxhblogspace_member.user_follow ta "
				"JOIN lxhblogspace_content.article_info tb ON ta.follower_uuid=? AND tb.user_uuid=ta.user_uuid AND tb.create_date <= ? AND tb.deleted=0 "
				"JOIN lxhblogspace_content.article_content tc ON tc.article_id=tb.article_id "
				"JOIN lxhblogspace_passport.user_info td ON td.user_uuid=tb.user_uuid "
				"JOIN lxhblogspace_passport.user_details te ON te.user_uuid=td.user_uuid "
				"ORDER BY tb.create_date DESC LIMIT ?,15")
			);
			statement->setString(1, request.GetCookieValue("_uuid"));
			statement->setInt64(2, time);
			statement->setInt64(3, index);
		}
		else
		{
			statement.reset(mysqlProperty.connection->prepareStatement(
				"SELECT tb.article_id, tc.title, tb.create_date, tb.vote_num, tc.content, td.username, te.avatar FROM lxhblogspace_member.user_follow ta "
				"JOIN lxhblogspace_content.article_info tb ON ta.follower_uuid=? AND tb.user_uuid=ta.user_uuid AND tb.deleted=0 "
				"JOIN lxhblogspace_content.article_content tc ON tc.article_id=tb.article_id "
				"JOIN lxhblogspace_passport.user_info td ON td.user_uuid=tb.user_uuid "
				"JOIN lxhblogspace_passport.user_details te ON te.user_uuid=td.user_uuid "
				"ORDER BY tb.create_date DESC LIMIT 15")
			);
			statement->setString(1, request.GetCookieValue("_uuid"));
		}

		nlohmann::json articleList(nlohmann::json::array());
		PtrResultSet result(statement->executeQuery());
		
		while (result->next())
		{
			std::string content = result->getString(5);
			long create_date = result->getInt(3);
			if (webstring::UTF8Strlen(content) > 100)
			{
				content = webstring::UTF8Substr(content, 0, 100);
			}

			//计算当前元素在查询结果中的偏移
			if (time == create_date)
			{
				index++;
			}
			else
			{
				index = 1;
				time = create_date;
			}
			
			articleList.push_back(
				{
					{"article_id", result->getInt(1)},
					{"title", webstring::Base64Encode(result->getString(2))},
					{"brief", webstring::Base64Encode(MarkdownBriefPretty(content))},
					{"create_date", create_date},
					{"interInfo", {{"vote", result->getInt(4)}, {"comments", GetArticleCommentNum(result->getString(1))}}},
					{"authorInfo", {{"name", result->getString(6)}, {"avatar", result->getString(7)}}},
					{"index", index}
				}
			);
		}
		
		response.body = articleList.dump();
		response.SetContentType("application/json; charset=utf8");

		return response;
	}
	catch (sql::SQLException e)
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::ServiceUnavailable);
		if (serverProperty.verbose >= VerboseLevel::essential)
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::GetFollowedArticleList(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()) + ", msg: " + e.what());
		}
		else
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::GetFollowedArticleList(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()));
		}
		return response;
	}

}

HTTPPacket::HTTPResponsePacket BlogSpaceContent::GetHottopic(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false)
{
	HTTPPacket::HTTPResponsePacket response;
	if (request.method != "GET")
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::MethodNotAllowed);
		return response;
	}

	try
	{
		mysqlProperty.connection->setSchema("lxhblogspace_content");
		PtrPreparedStatement selectSt(mysqlProperty.connection->prepareStatement("SELECT article_id, title FROM article_hottopic ORDER BY heat DESC"));
		PtrResultSet result(selectSt->executeQuery());

		std::vector<nlohmann::json> articleList;
		int len = 0;
		for (int len = 0; len < 10 && result->next(); ++len)
		{
			articleList.push_back(
				{
					{"article_id", result->getInt(1)},
					{"title", result->getString(2)}
				}
			);
		}
		nlohmann::json responseJson;
		responseJson["list_len"] = len;
		responseJson["list"] = articleList;

		response.SetContentType("application/json; charset=UTF-8");
		response.body = responseJson.dump();
		return response;
	}
	catch (sql::SQLException e)
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::ServiceUnavailable);
		if (serverProperty.verbose >= VerboseLevel::essential)
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::GetHottopic(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()) + ", msg: " + e.what());
		}
		else
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::GetHottopic(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()));
		}
		return response;
	}
}

HTTPPacket::HTTPResponsePacket BlogSpaceContent::GetFriendLink(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false)
{
	HTTPPacket::HTTPResponsePacket response;
	
	try
	{
		mysqlProperty.connection->setSchema("lxhblogspace_content");
		PtrStatement selectST(mysqlProperty.connection->createStatement());
		PtrResultSet result(selectST->executeQuery("SELECT sitename, href FROM friendlink ORDER BY priority DESC"));

		nlohmann::json links = nlohmann::json::array();
		while (result->next())
		{
			links.push_back
			(
				{
					{"title", result->getString("sitename")},
					{"href", result->getString("href")}
				}
			);
		}

		response.body = links.dump();
		response.SetContentType("application/json; charset=UTF-8");
		return response;
	}
	catch (sql::SQLException e)
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::ServiceUnavailable);
		if (serverProperty.verbose >= VerboseLevel::essential)
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::GetFriendLink(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()) + ", msg: " + e.what());
		}
		else
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::GetFriendLink(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()));
		}
		return response;
	}
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
	std::string schema = mysqlProperty.connection->getSchema();

	mysqlProperty.connection->setSchema("lxhblogspace_passport");
	PtrPreparedStatement statement(mysqlProperty.connection->prepareStatement("SELECT token_value, token_date FROM user_token WHERE user_uuid = ?"));
	statement->setString(1, uuid);

	PtrResultSet result(statement->executeQuery());

	if (schema != "")
	{
		mysqlProperty.connection->setSchema(schema);
	}

	while (result->next())
	{
		if (result->getString(1) == token)
		{
			//30天有效期
			time_t currentTime = time(nullptr);
			if (currentTime == -1)
			{
				break;
			}

			if ((currentTime - result->getInt64(2)) < 2592000)
			{
				logined = true;
				break;
			}
		}
	}

	if (logined == false)
	{
		return false;
	}

	mysqlProperty.connection->setSchema("lxhblogspace_passport");
	statement.reset(mysqlProperty.connection->prepareStatement("SELECT is_locked FROM user_info WHERE user_uuid = ?"));
	statement->setString(1, uuid);
	result.reset(statement->executeQuery());
	
	if (schema != "")
	{
		mysqlProperty.connection->setSchema(schema);
	}

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

std::string BlogSpaceContent::GetUUIDByUsername(std::string username) noexcept(false)
{
	std::string schema = mysqlProperty.connection->getSchema();

	mysqlProperty.connection->setSchema("lxhblogspace_passport");
	PtrPreparedStatement statement(mysqlProperty.connection->prepareStatement("SELECT user_uuid FROM user_info WHERE username=?"));
	statement->setString(1, username);
	PtrResultSet result(statement->executeQuery());

	if (schema != "")
	{
		mysqlProperty.connection->setSchema(schema);
	}

	while (result->next())
	{
		return result->getString("user_uuid");
	}

	return "";
}

std::string BlogSpaceContent::GetUserAvatar(std::string uuid) noexcept(false)
{
	std::string schema = mysqlProperty.connection->getSchema();

	mysqlProperty.connection->setSchema("lxhblogspace_passport");
	PtrPreparedStatement statement(mysqlProperty.connection->prepareStatement("SELECT avatar FROM user_details WHERE user_uuid=?"));
	statement->setString(1, uuid);
	PtrResultSet result(statement->executeQuery());

	if (schema != "")
	{
		mysqlProperty.connection->setSchema(schema);
	}

	while (result->next())
	{
		return result->getString("avatar");
	}

	return "";
}
std::string BlogSpaceContent::GetUsernameByUUID(std::string uuid) noexcept(false)
{
	std::string schema = mysqlProperty.connection->getSchema();

	mysqlProperty.connection->setSchema("lxhblogspace_passport");
	PtrPreparedStatement statement(mysqlProperty.connection->prepareStatement("SELECT username FROM user_info WHERE user_uuid=?"));
	statement->setString(1, uuid);
	PtrResultSet result(statement->executeQuery());

	if (schema != "")
	{
		mysqlProperty.connection->setSchema(schema);
	}

	while (result->next())
	{
		return result->getString("username");
	}

	return "";
}

int BlogSpaceContent::GetArticleCommentNum(std::string articleId) noexcept(false)
{
	std::string schema = mysqlProperty.connection->getSchema();

	mysqlProperty.connection->setSchema("lxhblogspace_content");
	PtrPreparedStatement statement(mysqlProperty.connection->prepareStatement("SELECT COUNT(*) FROM article_comment WHERE article_id=?"));
	statement->setString(1, articleId);
	PtrResultSet result(statement->executeQuery());
	result->next();

	if (schema != "")
	{
		mysqlProperty.connection->setSchema(schema);
	}

	return result->getInt(1);
	
}

std::string BlogSpaceContent::GetArticleTitle(std::string articleId) noexcept(false)
{
	std::string schema = mysqlProperty.connection->getSchema();

	mysqlProperty.connection->setSchema("lxhblogspace_content");
	PtrPreparedStatement statement(mysqlProperty.connection->prepareStatement("SELECT title FROM article_content WHERE article_id=?"));
	statement->setString(1, articleId);
	PtrResultSet result(statement->executeQuery());

	if (schema != "")
	{
		mysqlProperty.connection->setSchema(schema);
	}

	while (result->next())
	{
		return result->getString(1);
	}
	return "";
}

std::string BlogSpaceContent::GetArticleAuthor(std::string articleId) noexcept(false)
{
	std::string schema = mysqlProperty.connection->getSchema();

	mysqlProperty.connection->setSchema("lxhblogspace_content");
	PtrPreparedStatement statement(mysqlProperty.connection->prepareStatement("SELECT user_uuid FROM article_info WHERE article_id=?"));
	statement->setString(1, articleId);
	PtrResultSet result(statement->executeQuery());

	if (schema != "")
	{
		mysqlProperty.connection->setSchema(schema);
	}

	while (result->next())
	{
		return result->getString(1);
	}
	return "";	
}

bool BlogSpaceContent::IsArticleExist(std::string aritcleId) noexcept(false)
{
	std::string schema = mysqlProperty.connection->getSchema();

	mysqlProperty.connection->setSchema("lxhblogspace_content");
	PtrPreparedStatement statement(mysqlProperty.connection->prepareStatement("SELECT * FROM article_info WHERE article_id=?"));
	statement->setString(1, aritcleId);
	PtrResultSet result(statement->executeQuery());

	if (schema != "")
	{
		mysqlProperty.connection->setSchema(schema);
	}
	
	if (result->rowsCount() == 0)
	{
		return false;
	}
	{
		return true;
	}
}

bool BlogSpaceContent::IsVoted(std::string uuid, std::string articleId) noexcept(false)
{
	std::string schema = mysqlProperty.connection->getSchema();

	mysqlProperty.connection->setSchema("lxhblogspace_content");
	PtrPreparedStatement statement(mysqlProperty.connection->prepareStatement("SELECT * FROM article_vote_history WHERE user_uuid=? AND article_id=?"));
	statement->setString(1, uuid);
	statement->setString(2, articleId);
	PtrResultSet result(statement->executeQuery());

	if (schema != "")
	{
		mysqlProperty.connection->setSchema(schema);
	}

	if (result->rowsCount() == 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool BlogSpaceContent::IsSubscribed(std::string uuid, std::string articleId) noexcept(false)
{
	std::string schema = mysqlProperty.connection->getSchema();

	mysqlProperty.connection->setSchema("lxhblogspace_content");
	PtrPreparedStatement statement(mysqlProperty.connection->prepareStatement("SELECT * FROM article_subscribe_history WHERE user_uuid=? AND article_id=?"));
	statement->setString(1, uuid);
	statement->setString(2, articleId);
	PtrResultSet result(statement->executeQuery());

	if (schema != "")
	{
		mysqlProperty.connection->setSchema(schema);
	}

	if (result->rowsCount() == 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}

std::string BlogSpaceContent::MarkdownBriefPretty(std::string text) noexcept
{
	
	std::regex img_format("!\\[.*?\\]\\(.*?\\)");
	std::regex newline_format("\n");
	std::regex title_format("#+? ");
	std::regex linesplitor_format("____");
	std::regex code_format("`");

	std::regex special_chars("[<>\\|\\*]+");
	std::string result;
	result = std::regex_replace(text, img_format, "[图片] ");
	result = std::regex_replace(result, newline_format, " ");
	result = std::regex_replace(result, title_format, " ");
	result = std::regex_replace(result, linesplitor_format, " ");
	result = std::regex_replace(result, code_format, " ");
	result = std::regex_replace(result, special_chars, " ");

	return result;
}