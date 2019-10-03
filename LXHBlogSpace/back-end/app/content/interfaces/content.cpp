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

	RaiseHTPPError(clientfd, 404);
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

	if (webstring::UTF8Strlen(draftTitle) > 20 || webstring::UTF8Strlen(draftContent) >50000)
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
					statement->setInt64(1, time(nullptr));
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
		statement->setInt64(2, time(nullptr));
		statement->setInt64(3, time(nullptr));
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
	
	std::vector<std::string> draftList;
	try
	{
		if (CheckUserToken(request) == false)
		{
			response.SetResponseCode(HTTPPacket::ResponseCode::Forbidden);
			return response;
		}

		std::string uuid = request.GetCookieValue("_uuid");

		mysqlProperty.connection->setSchema("lxhblogspace_content");
		PtrPreparedStatement statement(mysqlProperty.connection->prepareStatement("SELECT draft_id, lastmodify_date, deleted FROM draft_info WHERE user_uuid=?"));
		statement->setString(1, uuid);
		PtrResultSet result(statement->executeQuery());
		while (result->next())
		{
			if (result->getUInt("deleted") == 1)
			{
				continue;
			}

			std::map<std::string, std::string> draft;
			draft["draft_id"] = result->getString(1);
			draft["lastmodify_date"] = result->getString(2);

			PtrPreparedStatement draftContentST(mysqlProperty.connection->prepareStatement("SELECT title, content FROM draft_content WHERE draft_id=?"));
			draftContentST->setString(1, draft["draft_id"]);
			PtrResultSet draftContent(draftContentST->executeQuery());
			while (draftContent->next())
			{
				draft["title"] = draftContent->getString(1);
				draft["content"] = draftContent->getString(2);
			}
			draftList.push_back(webstring::JsonStringify(draft));
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

	std::string result = "[";
	for (auto draft : draftList)
	{
		result += draft + ", ";
	}
	result = result.substr(0, result.length() - 2);
	result += "]";
	
	response.body = result;
	response.SetContentType("application/json; charset=UTF-8");

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

	if (webstring::UTF8Strlen(articleTitle) > 20 || webstring::UTF8Strlen(articleContent) > 50000)
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
					statement->setInt64(1, time(nullptr));
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
			responseJson.insert({ "reason", "指定文章不存在" });
			responseJson.insert({ "article_id", articleId });

			response.SetContentType("application/json; charset=UTF-8");
			response.body = webstring::JsonStringify(responseJson);
			return response;
		}

		PtrStatement transaction(mysqlProperty.connection->createStatement());
		transaction->execute("BEGIN");

		mysqlProperty.connection->setSchema("lxhblogspace_content");
		PtrPreparedStatement statement(mysqlProperty.connection->prepareStatement("INSERT INTO article_info(user_uuid, create_date, lastmodify_date, background_img, deleted) VALUES(?,?,?,?,?)"));

		statement->setString(1, request.GetCookieValue("_uuid"));
		statement->setInt64(2, time(nullptr));
		statement->setInt64(3, time(nullptr));
		statement->setString(4, backgroundPath);
		statement->setUInt(5, 0);
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
		responseJson.insert({ "article_id", articleId });

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
		PtrPreparedStatement statement(mysqlProperty.connection->prepareStatement("SELECT user_uuid, lastmodify_date, background_img, deleted FROM article_info WHERE article_id=?"));
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
			while (result->next())
			{
				responseJson["author"] = result->getString(1);
			}

			responseJson.insert({ "ecode", "0" });

			response.SetContentType("application/json; charset=UTF-8");
			response.body = webstring::JsonStringify(responseJson);
			return response;
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

			resultJson["ecode"] = "0";
			resultJson["reason"] = "删除成功";

			response.SetContentType("application/json; charset=UTF-8");
			response.body = webstring::JsonStringify(resultJson);
			return response;
		}
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

/*
HTTPPacket::HTTPResponsePacket BlogSpaceContent::GetArticleList(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false)
{
	HTTPPacket::HTTPResponsePacket response;
	response.SetServer(SERVER_SIGNATURE);

	if (request.method != "GET")
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::MethodNotAllowed);
		return response;
	}

	std::string username = request.ParseURLParamter()["user"];
	std::
	if (username == "")
	{
		
	}
}
*/

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

