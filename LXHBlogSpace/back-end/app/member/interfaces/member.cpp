#include "member.h"
#include <tinyHttpd/webstring.h>

#include <sstream>
#include <fstream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <nlohmann/json.hpp>

#define SERVER_SIGNATURE "tinyHttpd/0.1a"

void BlogSpaceMember::ModuleInit() noexcept(false)
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

void BlogSpaceMember::InitMysqlConnection() noexcept(false)
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

void BlogSpaceMember::HTTPPacketHandler(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept
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

	if (request.requestPath == "/api/v1/member/")
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::Found);
		response.SetLocation("http://blog.leaflxh.com");
		connectedClients[clientfd].writeBuffer += response.ToString();
		LogResponse(clientfd, request, response.code);

		return;
	}

	if (request.requestPath == "/api/v1/member/AddFollow")
	{
		try
		{
			response = AddFollow(clientfd, request);

			connectedClients[clientfd].writeBuffer += response.ToString();
			LogResponse(clientfd, request, response.code);
		}
		catch (std::runtime_error e)
		{
			LogRequestError(clientfd, request, e.what());
		}
		return;
	}

	if (request.requestPath == "/api/v1/member/RemoveFollow")
	{
		try
		{
			response = RemoveFollow(clientfd, request);

			connectedClients[clientfd].writeBuffer += response.ToString();
			LogResponse(clientfd, request, response.code);
		}
		catch (std::runtime_error e)
		{
			LogRequestError(clientfd, request, e.what());
		}
		return;
	}

	if (request.requestPath == "/api/v1/member/GetFollowerList")
	{
		try
		{
			response = GetFollowerList(clientfd, request);

			connectedClients[clientfd].writeBuffer += response.ToString();
			LogResponse(clientfd, request, response.code);
		}
		catch (std::runtime_error e)
		{
			LogRequestError(clientfd, request, e.what());
		}
		return;
	}

	if (request.requestPath == "/api/v1/member/GetFollowedList")
	{
		try
		{
			response = GetFollowedList(clientfd, request);

			connectedClients[clientfd].writeBuffer += response.ToString();
			LogResponse(clientfd, request, response.code);
		}
		catch (std::runtime_error e)
		{
			LogRequestError(clientfd, request, e.what());
		}
		return;
	}

	if (request.requestPath == "/api/v1/member/GetUserFollowInfo")
	{
		try
		{
			response = GetUserFollowInfo(clientfd, request);

			connectedClients[clientfd].writeBuffer += response.ToString();
			LogResponse(clientfd, request, response.code);
		}
		catch (std::runtime_error e)
		{
			LogRequestError(clientfd, request, e.what());
		}
		return;
	}

	if (request.requestPath == "/api/v1/member/GetUserMsgList")
	{
		try
		{
			response = GetUserMsgList(clientfd, request);

			connectedClients[clientfd].writeBuffer += response.ToString();
			LogResponse(clientfd, request, response.code);
		}
		catch (std::runtime_error e)
		{
			LogRequestError(clientfd, request, e.what());
		}
		return;
	}

	if (request.requestPath == "/api/v1/member/GetSystemMsgList")
	{
		try
		{
			response = GetSystemMsgList(clientfd, request);

			connectedClients[clientfd].writeBuffer += response.ToString();
			LogResponse(clientfd, request, response.code);
		}
		catch (std::runtime_error e)
		{
			LogRequestError(clientfd, request, e.what());
		}
		return;
	}

	if (request.requestPath == "/api/v1/member/GetUnreadedMsgNum")
	{
		try
		{
			response = GetUnreadedMsgNum(clientfd, request);

			connectedClients[clientfd].writeBuffer += response.ToString();
			LogResponse(clientfd, request, response.code);
		}
		catch (std::runtime_error e)
		{
			LogRequestError(clientfd, request, e.what());
		}
		return;
	}

	if (request.requestPath == "/api/v1/member/SetMessageReaded")
	{
		try
		{
			response = SetMessageReaded(clientfd, request);

			connectedClients[clientfd].writeBuffer += response.ToString();
			LogResponse(clientfd, request, response.code);
		}
		catch (std::runtime_error e)
		{
			LogRequestError(clientfd, request, e.what());
		}
		return;
	}

	if (request.requestPath == "/api/v1/member/SetUserMsgAllReaded")
	{
		try
		{
			response = SetUserMsgAllReaded(clientfd, request);

			connectedClients[clientfd].writeBuffer += response.ToString();
			LogResponse(clientfd, request, response.code);
		}
		catch (std::runtime_error e)
		{
			LogRequestError(clientfd, request, e.what());
		}
		return;
	}

	if (request.requestPath == "/api/v1/member/DeleteUserMsg")
	{
		try
		{
			response = DeleteUserMsg(clientfd, request);

			connectedClients[clientfd].writeBuffer += response.ToString();
			LogResponse(clientfd, request, response.code);
		}
		catch (std::runtime_error e)
		{
			LogRequestError(clientfd, request, e.what());
		}
		return;
	}

	if (request.requestPath == "/api/v1/member/DeleteUserMsgAll")
	{
		try
		{
			response = DeleteUserMsgAll(clientfd, request);

			connectedClients[clientfd].writeBuffer += response.ToString();
			LogResponse(clientfd, request, response.code);
		}
		catch (std::runtime_error e)
		{
			LogRequestError(clientfd, request, e.what());
		}
		return;
	}

	if (request.requestPath == "/api/v1/member/SetWatchedHistory")
	{
		try
		{
			response = SetWatchedHistory(clientfd, request);

			connectedClients[clientfd].writeBuffer += response.ToString();
			LogResponse(clientfd, request, response.code);
		}
		catch (std::runtime_error e)
		{
			LogRequestError(clientfd, request, e.what());
		}
		return;
	}

	if (request.requestPath == "/api/v1/member/GetExplored")
	{
		try
		{
			response = GetExplored(clientfd, request);

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

HTTPPacket::HTTPResponsePacket BlogSpaceMember::AddFollow(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false)
{
	HTTPPacket::HTTPResponsePacket response;
	response.SetServer(SERVER_SIGNATURE);

	if (request.method != "POST")
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::MethodNotAllowed);
		return response;
	}

	auto requestParam = webstring::ParseKeyValue(request.body);
	std::map<std::string, std::string> responseJson;

	try
	{
		if (CheckUserToken(request) == false)
		{
			response.SetResponseCode(HTTPPacket::ResponseCode::Forbidden);
			return response;
		}

		std::string followedUuid = GetUUIDByUsername(webstring::URLdecode(requestParam["username"]));
		if (followedUuid == "")
		{
			responseJson["ecode"] = "-2";
			responseJson["reason"] = "指定用户不存在";

			response.SetContentType("application/json; charset=UTF-8");;
			response.body = webstring::JsonStringify(responseJson);
			return response;
		}

		if (followedUuid == request.GetCookieValue("_uuid"))
		{
			responseJson["ecode"] = "-6";
			responseJson["reason"] = "用户不能关注自己";

			response.SetContentType("application/json; charset=UTF-8");;
			response.body = webstring::JsonStringify(responseJson);
			return response;
		}

		PtrPreparedStatement followST(mysqlProperty.connection->prepareStatement("SELECT user_uuid, follower_uuid FROM user_follow WHERE user_uuid=? AND follower_uuid=?"));
		followST->setString(1, followedUuid);
		followST->setString(2, request.GetCookieValue("_uuid"));
		PtrResultSet result(followST->executeQuery());
		if (result->rowsCount() != 0)
		{
			responseJson["ecode"] = "-7";
			responseJson["reason"] = "已关注该用户";

			response.SetContentType("application/json; charset=UTF-8");;
			response.body = webstring::JsonStringify(responseJson);
			return response;
		}

		//使用事务做同步
		mysqlProperty.connection->setSchema("lxhblogspace_member");
		PtrStatement transactionST(mysqlProperty.connection->createStatement());
		transactionST->execute("BEGIN");

		//添加关注
		followST.reset(mysqlProperty.connection->prepareStatement("INSERT INTO user_follow(user_uuid, follower_uuid) VALUES(?,?)"));
		followST->setString(1, followedUuid);
		followST->setString(2, request.GetCookieValue("_uuid"));
		followST->execute();

		//更新关注者的关注数量
		PtrPreparedStatement updateST(mysqlProperty.connection->prepareStatement("UPDATE user_follow_info JOIN (SELECT COUNT(*) as num FROM user_follow WHERE follower_uuid=?) b SET followed_num=b.num WHERE user_uuid=?"));
		updateST->setString(1, request.GetCookieValue("_uuid"));
		updateST->setString(2, request.GetCookieValue("_uuid"));
		if (updateST->executeUpdate() == 0)
		{
			updateST.reset(mysqlProperty.connection->prepareStatement("INSERT INTO user_follow_info(user_uuid, fans_num, followed_num) VALUES(?,?,?)"));
			updateST->setString(1, request.GetCookieValue("_uuid"));
			updateST->setString(2, "0");
			updateST->setString(3, "1");
			updateST->execute();
		}

		//更新被关注者的粉丝数量
		updateST.reset(mysqlProperty.connection->prepareStatement("UPDATE user_follow_info JOIN (SELECT COUNT(*) as num FROM user_follow WHERE user_uuid=?) b SET fans_num=b.num WHERE user_uuid=?"));
		updateST->setString(1, followedUuid);
		updateST->setString(2, followedUuid);
		if (updateST->executeUpdate() == 0)
		{
			updateST.reset(mysqlProperty.connection->prepareStatement("INSERT INTO user_follow_info(user_uuid, fans_num, followed_num) VALUES(?,?,?)"));
			updateST->setString(1, followedUuid);
			updateST->setString(2, "1");
			updateST->setString(3, "0");
			updateST->execute();
		}

		transactionST->execute("COMMIT");

		responseJson["ecode"] = "0";
		responseJson["reason"] = "关注成功";

		response.SetContentType("application/json; charset=UTF-8");;
		response.body = webstring::JsonStringify(responseJson);
		return response;

	}
	catch (sql::SQLException e)
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::ServiceUnavailable);
		if (serverProperty.verbose >= VerboseLevel::essential)
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::AddFollow(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()) + ", msg: " + e.what());
		}
		else
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::AddFollow(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()));
		}
		return response;
	}
}

HTTPPacket::HTTPResponsePacket BlogSpaceMember::RemoveFollow(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false)
{
	HTTPPacket::HTTPResponsePacket response;
	response.SetServer(SERVER_SIGNATURE);

	if (request.method != "POST")
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::MethodNotAllowed);
		return response;
	}

	auto requestParam = webstring::ParseKeyValue(request.body);
	std::map<std::string, std::string> responseJson;

	try
	{
		if (CheckUserToken(request) == false)
		{
			response.SetResponseCode(HTTPPacket::ResponseCode::Forbidden);
			return response;
		}

		std::string followedUuid = GetUUIDByUsername(webstring::URLdecode(requestParam["username"]));
		if (followedUuid == "")
		{
			responseJson["ecode"] = "-2";
			responseJson["reason"] = "指定用户不存在";

			response.SetContentType("application/json; charset=UTF-8");;
			response.body = webstring::JsonStringify(responseJson);
			return response;
		}

		mysqlProperty.connection->setSchema("lxhblogspace_member");
		PtrPreparedStatement statement(mysqlProperty.connection->prepareStatement("DELETE FROM user_follow WHERE user_uuid=? AND follower_uuid=?"));
		statement->setString(1, followedUuid);
		statement->setString(2, request.GetCookieValue("_uuid"));
		statement->execute();

		//更新被关注者的粉丝数
		statement.reset(mysqlProperty.connection->prepareStatement("UPDATE user_follow_info JOIN (SELECT COUNT(*) as num FROM user_follow WHERE user_uuid=?) b SET fans_num=b.num WHERE user_uuid=?"));
		statement->setString(1, followedUuid);
		statement->setString(2, followedUuid);
		statement->executeUpdate();

		//更新关注者的关注数量
		statement.reset(mysqlProperty.connection->prepareStatement("UPDATE user_follow_info JOIN (SELECT COUNT(*) as num FROM user_follow WHERE follower_uuid=?) b SET followed_num=b.num WHERE user_uuid=?"));
		statement->setString(1, request.GetCookieValue("_uuid"));
		statement->setString(2, request.GetCookieValue("_uuid"));
		statement->executeUpdate();

		responseJson["ecode"] = "0";
		responseJson["reason"] = "已取消关注";

		response.SetContentType("application/json; charset=UTF-8");;
		response.body = webstring::JsonStringify(responseJson);
		return response;

	}
	catch (sql::SQLException e)
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::ServiceUnavailable);
		if (serverProperty.verbose >= VerboseLevel::essential)
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::RemoveFollow(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()) + ", msg: " + e.what());
		}
		else
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::RemoveFollow(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()));
		}
		return response;
	}
}

HTTPPacket::HTTPResponsePacket BlogSpaceMember::GetFollowerList(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false)
{
	HTTPPacket::HTTPResponsePacket response;
	response.SetServer(SERVER_SIGNATURE);

	nlohmann::json responseJson;

	auto requestParams = request.ParseURLParamter();
	std::string username = webstring::URLdecode(requestParams["username"]);
	int requestPage = 1;
	try
	{
		if (requestParams.count("page") != 0)
		{
			requestPage = std::stoi(requestParams["page"]);
			if (requestPage <= 0)
			{
				requestPage = 1;
			}
		}
	}
	catch (...)
	{
		responseJson["ecode"] = -1;
		responseJson["reaspon"] = "请求参数有误";

		response.SetContentType("application/json; charset=UTF-8");
		response.body = responseJson.dump();
		return response;
	}

	if (username == "")
	{
		responseJson["ecode"] = -1;
		responseJson["reaspon"] = "请求参数有误"; 

		response.SetContentType("application/json; charset=UTF-8");
		response.body = responseJson.dump();
		return response;
	}

	try
	{
		std::string queriedUserUUID = GetUUIDByUsername(username);
		if (queriedUserUUID == "")
		{
			responseJson["ecode"] = -2;
			responseJson["reason"] = "查询的用户不存在";

			response.SetContentType("application/json; charset=UTF-8");
			response.body = responseJson.dump();

			return response;
		}

		bool loggedin = CheckUserToken(request);
		std::string currentUserUUID = request.GetCookieValue("_uuid");

		std::vector<nlohmann::json> userList;
		mysqlProperty.connection->setSchema("lxhblogspace_member");
		PtrPreparedStatement queryStatement(mysqlProperty.connection->prepareStatement("SELECT follower_uuid FROM user_follow WHERE user_uuid=? LIMIT ?,9"));
		queryStatement->setString(1, queriedUserUUID);
		queryStatement->setInt(2, (requestPage - 1) * 9);
		PtrResultSet result(queryStatement->executeQuery());
		while (result->next())
		{
			std::string followerUUID = result->getString(1);
			bool followed = false;
			if (loggedin == true)
			{
				PtrPreparedStatement followST(mysqlProperty.connection->prepareStatement("SELECT * FROM user_follow WHERE user_uuid=? AND follower_uuid=?"));
				followST->setString(1, followerUUID);
				followST->setString(2, currentUserUUID);
				PtrResultSet followInfo(followST->executeQuery());
				if (followInfo->rowsCount() != 0)
				{
					followed = true;
				}
			}

			userList.push_back({
				{"username", GetUsernameByUUID(followerUUID)},
				{"avatar", GetUserAvatar(followerUUID)},
				{"followed", followed}
			});
		}

		responseJson["ecode"] = 0;
		responseJson["user_list"] = userList;

		response.SetContentType("application/json; charset=UTF-8");
		response.body = responseJson.dump();

		return response;
	}
	catch (sql::SQLException e)
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::ServiceUnavailable);
		if (serverProperty.verbose >= VerboseLevel::essential)
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::GetFollowerList(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()) + ", msg: " + e.what());
		}
		else
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::GetFollowerList(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()));
		}
		return response;
	}
}

HTTPPacket::HTTPResponsePacket BlogSpaceMember::GetFollowedList(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false)
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

	nlohmann::json responseJson;
	auto requestParam = request.ParseURLParamter();
	std::string username = webstring::URLdecode(requestParam["username"]);
	int requestPage = 1;
	if (requestParam.count("page") != 0)
	{
		try
		{
			requestPage = std::stoi(requestParam["page"]);
			if (requestPage <= 0)
			{
				requestPage = 1;
			}
		}
		catch (...)
		{
			responseJson["ecode"] = -1;
			responseJson["reaspon"] = " 请求参数有误";

			response.SetContentType("application/json; charset=UTF-8");
			response.body = responseJson.dump();
			return response;
		}
	}

	if (username == "")
	{
		responseJson["ecode"] = -1;
		responseJson["reaspon"] = " 请求参数有误";

		response.SetContentType("application/json; charset=UTF-8");
		response.body = responseJson.dump();
		return response;
	}

	try
	{
		std::string queriedUserUUID = GetUUIDByUsername(username);
		if (queriedUserUUID == "")
		{
			responseJson["ecode"] = -2;
			responseJson["reason"] = "查询的用户不存在";

			response.SetContentType("applicatoin/json; charset=UTF-8");
			response.body = responseJson.dump();

			return response;
		}

		std::vector<nlohmann::json> userList;
		mysqlProperty.connection->setSchema("lxhblogspace_member");
		PtrPreparedStatement queryStatement(mysqlProperty.connection->prepareStatement("SELECT user_uuid FROM user_follow WHERE follower_uuid=? LIMIT ?,9"));
		queryStatement->setString(1, queriedUserUUID);
		queryStatement->setInt(2, (requestPage - 1) * 9);

		PtrResultSet result(queryStatement->executeQuery());
		bool loggedin = CheckUserToken(request);
		std::string currentUser = request.GetCookieValue("_uuid");
		while (result->next())
		{
			std::string followedUserUUID = result->getString(1);
			bool followed = false;
			if (loggedin == true)
			{
				PtrPreparedStatement followST(mysqlProperty.connection->prepareStatement("SELECT * FROM user_follow WHERE user_uuid=? AND follower_uuid=?"));
				followST->setString(1, followedUserUUID);
				followST->setString(2, currentUser);
				PtrResultSet followInfo(followST->executeQuery());
				if (followInfo->rowsCount() != 0)
				{
					followed = true;
				}
			}

			userList.push_back({
				{"username", GetUsernameByUUID(followedUserUUID)},
				{"avatar", GetUserAvatar(followedUserUUID)},
				{"followed", followed}
			});
		}

		responseJson["ecode"] = 0;
		responseJson["user_list"] = userList;

		response.SetContentType("application/json; charset=UTF-8");
		response.body = responseJson.dump();

		return response;
	}
	catch (sql::SQLException e)
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::ServiceUnavailable);
		if (serverProperty.verbose >= VerboseLevel::essential)
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::GetFollowedList(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()) + ", msg: " + e.what());
		}
		else
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::GetFollowedList(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()));
		}
		return response;
	}
}

HTTPPacket::HTTPResponsePacket BlogSpaceMember::GetUserFollowInfo(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false)
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

	std::map<std::string, std::string> responseJson;
	std::string username = request.ParseURLParamter()["username"];
	username = webstring::URLdecode(username);

	if (username == "")
	{
		responseJson["ecode"] = "-1";
		responseJson["reaspon"] = " 请求参数有误";

		response.SetContentType("application/json; charset=UTF-8");
		response.body = webstring::JsonStringify(responseJson);
		return response;
	}

	try
	{
		std::string queriedUserUUID = GetUUIDByUsername(username);
		if (queriedUserUUID == "")
		{
			responseJson["ecode"] = "-2";
			responseJson["reason"] = "查询的用户不存在";

			response.SetContentType("applicatoin/json; charset=UTF-8");
			response.body = webstring::JsonStringify(responseJson);

			return response;
		}

		ptree userListRoot;
		mysqlProperty.connection->setSchema("lxhblogspace_member");
		PtrPreparedStatement queryStatement(mysqlProperty.connection->prepareStatement("SELECT fans_num, followed_num FROM user_follow_info WHERE user_uuid=?"));
		queryStatement->setString(1, queriedUserUUID);
		PtrResultSet result(queryStatement->executeQuery());

		responseJson["ecode"] = "0";
		if (result->rowsCount() == 0)
		{
			responseJson["fans_num"] = "0";
			responseJson["followed_num"] = "0";
		}
		else
		{
			result->next();
			responseJson["fans_num"] = result->getString(1);
			responseJson["followed_num"] = result->getString(2);
		}

		if (CheckUserToken(request) == true)
		{
			queryStatement.reset(mysqlProperty.connection->prepareStatement("SELECT * FROM user_follow WHERE user_uuid=? AND follower_uuid=?"));
			queryStatement->setString(1, queriedUserUUID);
			queryStatement->setString(2, request.GetCookieValue("_uuid"));

			result.reset(queryStatement->executeQuery());
			if (result->rowsCount() > 0)
			{
				responseJson["following"] = "true";
			}
			else
			{
				responseJson["following"] = "false";
			}
		}

		response.SetContentType("application/json; charset=UTF-8");
		response.body = webstring::JsonStringify(responseJson);

		return response;
	}
	catch (sql::SQLException e)
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::ServiceUnavailable);
		if (serverProperty.verbose >= VerboseLevel::essential)
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::GetUserFollowInfo(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()) + ", msg: " + e.what());
		}
		else
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::GetUserFollowInfo(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()));
		}
		return response;
	}
}

HTTPPacket::HTTPResponsePacket BlogSpaceMember::GetUserMsgList(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false)
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

	nlohmann::json responseJsonRootTree;
	std::vector<nlohmann::json> msgList;

	try
	{
		if (CheckUserToken(request) == false)
		{
			response.SetResponseCode(HTTPPacket::ResponseCode::Forbidden);
			return response;
		}
		std::string useruuid = request.GetCookieValue("_uuid");

		mysqlProperty.connection->setSchema("lxhblogspace_member");
		PtrPreparedStatement userNoticeST(mysqlProperty.connection->prepareStatement("SELECT tb.from_uuid, tc.article_id, tc.title, tb.content, tb.comment_id, tb.create_date, ta.notice_id, ta.readed FROM  user_comment_notice ta JOIN lxhblogspace_content.article_comment tb ON ta.comment_id=tb.comment_id AND ta.recipient_uuid=? AND ta.deleted=0 JOIN lxhblogspace_content.article_content tc ON tc.article_id=tb.article_id ORDER BY ta.notice_id DESC"));
		userNoticeST->setString(1, useruuid);
		PtrResultSet userNotice(userNoticeST->executeQuery());

		while (userNotice->next())
		{
			msgList.push_back
			({
				{ "notice_id", userNotice->getUInt("notice_id") },
				{ "title", userNotice->getString("title") },
				{ "article_id", userNotice->getString("article_id") },
				{ "from", GetUsernameByUUID(userNotice->getString("from_uuid")) },
				{ "content", userNotice->getString("content") },
				{ "time", userNotice->getString("create_date") },
				{ "readed", userNotice->getString("readed") }
			});
		}
		responseJsonRootTree["msg_list"] = msgList;

		responseJsonRootTree["ecode"] = 0;
		response.SetContentType("application/json; charset=UTF-8");

		response.body = responseJsonRootTree.dump();

		return response;
	}
	catch (sql::SQLException e)
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::ServiceUnavailable);
		if (serverProperty.verbose >= VerboseLevel::essential)
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::GetUserMsgList(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()) + ", msg: " + e.what());
		}
		else
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::GetUserMsgList(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()));
		}
		return response;
	}
}

HTTPPacket::HTTPResponsePacket BlogSpaceMember::GetSystemMsgList(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false)
{
	HTTPPacket::HTTPResponsePacket response;
	response.SetServer(SERVER_SIGNATURE);

	if (request.method != "GET")
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::MethodNotAllowed);
		return response;
	}

	try
	{
		if (CheckUserToken(request) == false)
		{
			response.SetResponseCode(HTTPPacket::ResponseCode::Forbidden);
			return response;
		}

		std::string useruuid = request.GetCookieValue("_uuid");
		mysqlProperty.connection->setSchema("lxhblogspace_member");
		PtrPreparedStatement statement(mysqlProperty.connection->prepareStatement("SELECT * FROM sys_notice WHERE recipient_uuid=? ORDER BY notice_id DESC"));
		statement->setString(1, useruuid);
		PtrResultSet result(statement->executeQuery());

		std::vector<nlohmann::json> msgList;
		nlohmann::json responseJson;

		responseJson["ecode"] =  "0";

		while (result->next())
		{
			msgList.push_back
			({
				{"notice_id", result->getUInt("notice_id")},
				{"title",result->getString("msg_title")},
				{"content", result->getString("msg")},
				{"time", result->getInt64("create_date")},
				{"readed", result->getInt("readed")}
			});
		}
		responseJson["msg_list"] = msgList;

		response.body = responseJson.dump();
		response.SetContentType("application/json; charset=UTF-8");

		return response;
	}
	catch (sql::SQLException e)
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::ServiceUnavailable);
		if (serverProperty.verbose >= VerboseLevel::essential)
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::GetSystemMsgList(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()) + ", msg: " + e.what());
		}
		else
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::GetSystemMsgList(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()));
		}
		return response;
	}
}

HTTPPacket::HTTPResponsePacket BlogSpaceMember::GetUnreadedMsgNum(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false)
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

	try
	{
		if (CheckUserToken(request) == false)
		{
			response.SetResponseCode(HTTPPacket::ResponseCode::Forbidden);
			return response;
		}

		std::string useruuid = request.GetCookieValue("_uuid");
		ptree responseJson;
		responseJson.put("ecode", 0);

		mysqlProperty.connection->setSchema("lxhblogspace_member");
		PtrPreparedStatement statement(mysqlProperty.connection->prepareStatement("SELECT COUNT(*) FROM user_comment_notice WHERE recipient_uuid=? AND readed=0"));
		statement->setString(1, useruuid);
		PtrResultSet countResult(statement->executeQuery());
		countResult->next();
		
		responseJson.put("comment_msgs", countResult->getUInt(1));

		statement.reset(mysqlProperty.connection->prepareStatement("SELECT COUNT(*) FROM sys_notice WHERE recipient_uuid=? AND readed=0"));
		statement->setString(1, useruuid);
		countResult.reset(statement->executeQuery());
		countResult->next();
		responseJson.put("system_msgs", countResult->getUInt(1));

		std::stringstream stream;
		write_json(stream, responseJson, false);

		response.SetContentType("application/json; charset=UTF-8");
		response.body = stream.str();

		return response;
	}
	catch (sql::SQLException e)
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::ServiceUnavailable);
		if (serverProperty.verbose >= VerboseLevel::essential)
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::GetUnreadedMsgNum(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()) + ", msg: " + e.what());
		}
		else
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::GetUnreadedMsgNum(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()));
		}
		return response;
	}
}

HTTPPacket::HTTPResponsePacket BlogSpaceMember::SetMessageReaded(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false)
{
	using boost::property_tree::ptree;

	HTTPPacket::HTTPResponsePacket response;
	response.SetServer(SERVER_SIGNATURE);

	if (request.method != "POST")
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::MethodNotAllowed);
		return response;
	}

	try
	{
		if (CheckUserToken(request) == false)
		{
			response.SetResponseCode(HTTPPacket::ResponseCode::Forbidden);
			return response;
		}

		auto postParams = webstring::ParseKeyValue(request.body);
		ptree responseJson;

		PtrPreparedStatement statement;
		mysqlProperty.connection->setSchema("lxhblogspace_member");
		if (postParams["type"] == "user")
		{
			statement.reset(mysqlProperty.connection->prepareStatement("UPDATE user_comment_notice SET readed=1 WHERE recipient_uuid=? AND notice_id=?"));
		}
		else if (postParams["type"] == "system")
		{
			statement.reset(mysqlProperty.connection->prepareStatement("UPDATE sys_notice SET readed=1 WHERE recipient_uuid=? AND notice_id=?"));
		}
		else
		{
			responseJson.put("ecode", -4);
			responseJson.put("reason", "在设置消息已读时传递的消息类型有误");

			response.body = webstring::JsonStringify(responseJson);
			response.SetContentType("application/json; charset=UTF-8");
			return response;
		}

		statement->setString(1, request.GetCookieValue("_uuid"));
		statement->setString(2, postParams["notice_id"]);

		if (statement->executeUpdate() == 0)
		{
			responseJson.put("ecode", -3);
			responseJson.put("reason", "指定消息不存在");

			response.body = webstring::JsonStringify(responseJson);
			response.SetContentType("application/json; charset=UTF-8");
			return response;
		}

		responseJson.put("ecode", 0);

		response.body = webstring::JsonStringify(responseJson);
		response.SetContentType("application/json; charset=UTF-8");
		return response;
	}
	catch (sql::SQLException e)
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::ServiceUnavailable);
		if (serverProperty.verbose >= VerboseLevel::essential)
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::SetMessageReaded(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()) + ", msg: " + e.what());
		}
		else
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::SetMessageReaded(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()));
		}
		return response;
	}
}

HTTPPacket::HTTPResponsePacket BlogSpaceMember::SetUserMsgAllReaded(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false)
{
	HTTPPacket::HTTPResponsePacket response;
	response.SetServer(SERVER_SIGNATURE);

	if (request.method != "POST")
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::MethodNotAllowed);
		return response;
	}

	try
	{
		if (CheckUserToken(request) == false)
		{
			response.SetResponseCode(HTTPPacket::ResponseCode::Forbidden);
			return response;
		}

		nlohmann::json responseJson;

		mysqlProperty.connection->setSchema("lxhblogspace_member");
		PtrPreparedStatement statement(mysqlProperty.connection->prepareStatement("UPDATE user_comment_notice SET readed=1 WHERE recipient_uuid=?"));
		statement->setString(1, request.GetCookieValue("_uuid"));
		statement->execute();

		responseJson["ecode"]= 0;

		response.body = responseJson.dump();
		response.SetContentType("application/json; charset=UTF-8");
		return response;
	}
	catch (sql::SQLException e)
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::ServiceUnavailable);
		if (serverProperty.verbose >= VerboseLevel::essential)
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::SetUserMsgAllReaded(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()) + ", msg: " + e.what());
		}
		else
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::SetUserMsgAllReaded(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()));
		}
		return response;
	}

}

HTTPPacket::HTTPResponsePacket BlogSpaceMember::DeleteUserMsg(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false)
{
	using boost::property_tree::ptree;

	HTTPPacket::HTTPResponsePacket response;
	response.SetServer(SERVER_SIGNATURE);

	if (request.method != "POST")
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::MethodNotAllowed);
		return response;
	}

	try
	{
		if (CheckUserToken(request) == false)
		{
			response.SetResponseCode(HTTPPacket::ResponseCode::Forbidden);
			return response;
		}

		auto postParams = webstring::ParseKeyValue(request.body);
		nlohmann::json responseJson;

		mysqlProperty.connection->setSchema("lxhblogspace_member");
		PtrPreparedStatement statement(mysqlProperty.connection->prepareStatement("UPDATE user_comment_notice SET deleted=1 WHERE recipient_uuid=? AND notice_id=?"));
		statement->setString(1, request.GetCookieValue("_uuid"));
		statement->setString(2, postParams["notice_id"]);

		if (statement->executeUpdate() == 0)
		{
			responseJson["ecode"] =  -3;
			responseJson["reason"] = "指定消息不存在/消息已经被删除";

			response.body = responseJson.dump();
			response.SetContentType("application/json; charset=UTF-8");
			return response;
		}

		responseJson["ecode"] = 0;

		response.body = responseJson.dump();
		response.SetContentType("application/json; charset=UTF-8");
		return response;
	}
	catch (sql::SQLException e)
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::ServiceUnavailable);
		if (serverProperty.verbose >= VerboseLevel::essential)
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::DeleteUserMsg(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()) + ", msg: " + e.what());
		}
		else
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::DeleteUserMsg(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()));
		}
		return response;
	}
}

HTTPPacket::HTTPResponsePacket BlogSpaceMember::DeleteUserMsgAll(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false)
{
	using boost::property_tree::ptree;

	HTTPPacket::HTTPResponsePacket response;
	response.SetServer(SERVER_SIGNATURE);

	if (request.method != "POST")
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::MethodNotAllowed);
		return response;
	}

	try
	{
		if (CheckUserToken(request) == false)
		{
			response.SetResponseCode(HTTPPacket::ResponseCode::Forbidden);
			return response;
		}

		nlohmann::json responseJson;

		mysqlProperty.connection->setSchema("lxhblogspace_member");
		PtrPreparedStatement statement(mysqlProperty.connection->prepareStatement("UPDATE user_comment_notice SET deleted=1 WHERE recipient_uuid=?"));
		statement->setString(1, request.GetCookieValue("_uuid"));
		statement->execute();

		responseJson["ecode"] = 0;

		response.body = responseJson.dump();
		response.SetContentType("application/json; charset=UTF-8");
		return response;
	}
	catch (sql::SQLException e)
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::ServiceUnavailable);
		if (serverProperty.verbose >= VerboseLevel::essential)
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::DeleteUserMsgAll(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()) + ", msg: " + e.what());
		}
		else
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::DeleteUserMsgAll(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()));
		}
		return response;
	}
}

HTTPPacket::HTTPResponsePacket BlogSpaceMember::SetWatchedHistory(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false)
{
	HTTPPacket::HTTPResponsePacket response;
	response.SetServer(SERVER_SIGNATURE);

	if (request.method != "POST")
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::MethodNotAllowed);
		return response;
	}

	try
	{
		if (CheckUserToken(request) == false)
		{
			response.SetResponseCode(HTTPPacket::ResponseCode::Forbidden);
			return response;
		}

		boost::property_tree::ptree responseJson;

		std::string useruuid = request.GetCookieValue("_uuid");
		std::string articleId = webstring::ParseKeyValue(request.body)["article_id"];
		if (IsArticleExist(articleId) == false)
		{
			responseJson.put("ecode", -5);
			responseJson.put("reason", "指定文章不存在");

			response.body = webstring::JsonStringify(responseJson);
			response.SetContentType("application/json; charset=UTF-8");

			return response;
		}

		PtrPreparedStatement statement(mysqlProperty.connection->prepareStatement("INSERT INTO user_watch_history VALUES(?,?,?)"));
		statement->setString(1, useruuid);
		statement->setString(2, articleId);
		statement->setString(3, webstring::GenTimeStamp());
		statement->execute();

		responseJson.put("ecode", 0);
		response.SetContentType("application/json; charset=UTF-8");

		return response;

	}
	catch (sql::SQLException e)
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::ServiceUnavailable);
		if (serverProperty.verbose >= VerboseLevel::essential)
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::SetWatchedHistory(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()) + ", msg: " + e.what());
		}
		else
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::SetWatchedHistory(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()));
		}
		return response;
	}
}


HTTPPacket::HTTPResponsePacket BlogSpaceMember::GetExplored(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false)
{
	using nlohmann::json;

	HTTPPacket::HTTPResponsePacket response;
	response.SetServer(SERVER_SIGNATURE);

	if (request.method != "GET")
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::MethodNotAllowed);
		return response;
	}

	try
	{
		if (CheckUserToken(request) == false)
		{
			response.SetResponseCode(HTTPPacket::ResponseCode::Forbidden);
			return response;
		}

		std::string useruuid = request.GetCookieValue("_uuid");
		nlohmann::json responseJson;
		std::vector<nlohmann::json> historyInfoList;

		PtrPreparedStatement statement(mysqlProperty.connection->prepareStatement("SELECT a.article_id, a.watch_date b.title FROM user_watch_history a JOIN lxhblogspace_content.article_content b ON b.article_id=a.article_id AND a.uuid=? ORDER BY a.watch_date DESC"));
		statement->setString(1, useruuid);
		PtrResultSet result(statement->executeQuery());
		responseJson["ecode"] = 0;
		
		while (result->next())
		{
			historyInfoList.push_back
			({
				{"article_id", result->getString("article_id")},
				{"title", result->getString("title")},
				{"time", result->getUInt("watch_date")}
			});
		}
		responseJson["history_list"] = historyInfoList;

		response.body = responseJson.dump();
		response.SetContentType("application/json");

		return response;
	}
	catch (sql::SQLException e)
	{
		response.SetResponseCode(HTTPPacket::ResponseCode::ServiceUnavailable);
		if (serverProperty.verbose >= VerboseLevel::essential)
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::GetExplored(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()) + ", msg: " + e.what());
		}
		else
		{
			LogRequestError(clientfd, request, std::string("BlogSpaceContent::GetExplored(): MySQL exception occured with code ") + std::to_string(e.getErrorCode()));
		}
		return response;
	}
}

std::string BlogSpaceMember::GetUUIDByUsername(std::string username) noexcept(false)
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


std::string BlogSpaceMember::GetUserAvatar(std::string uuid) noexcept(false)
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

std::string BlogSpaceMember::GetUsernameByUUID(std::string uuid) noexcept(false)
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

bool BlogSpaceMember::IsArticleExist(std::string aritcleId) noexcept(false)
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

bool BlogSpaceMember::CheckUserToken(HTTPPacket::HTTPRequestPacket& request) noexcept(false)
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

	return true;
}
