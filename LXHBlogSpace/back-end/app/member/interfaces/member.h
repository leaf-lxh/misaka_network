#include <tinyHttpd/tinyHttpd.h>

#include <tinyHttpd/HTTPPacket.h>

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>

class BlogSpaceMember : public TinyHttpd
{
public:
	void ModuleInit() noexcept(false);


private:
	///MYSQL服务的属性
	struct MySQLProperty
	{
		//ip:port类型
		std::string host;
		std::string user;
		std::string password;
		sql::Driver* driverInstance;
		std::unique_ptr<sql::Connection> connection;
	}mysqlProperty;

	/*!
	仅初始化MYSQL数据库连接。如需重新连接请使用reconnect方法
	异常：初始化失败（如无法连接数据库，账密错误等），抛出runtime_error
	*/
	void InitMysqlConnection() noexcept(false);

	///MySQL对象指针的包装
	using PtrPreparedStatement = std::unique_ptr<sql::PreparedStatement>;
	using PtrStatement = std::unique_ptr<sql::Statement>;
	using PtrResultSet = std::unique_ptr<sql::ResultSet>;

	/*!
	本服务的HTTP报文处理器
	*/
	void HTTPPacketHandler(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept;

	/****************外部公开接口***************/
	HTTPPacket::HTTPResponsePacket AddFollow(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false);
	HTTPPacket::HTTPResponsePacket RemoveFollow(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false);
	HTTPPacket::HTTPResponsePacket GetFollowerList(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false);
	HTTPPacket::HTTPResponsePacket GetFollowedList(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false);
	HTTPPacket::HTTPResponsePacket GetUserFollowInfo(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false);

	HTTPPacket::HTTPResponsePacket GetUserMsgList(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false);
	HTTPPacket::HTTPResponsePacket GetSystemMsgList(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false);

	HTTPPacket::HTTPResponsePacket GetUnreadedMsgNum(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false);

	HTTPPacket::HTTPResponsePacket SetMessageReaded(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false);
	HTTPPacket::HTTPResponsePacket SetWatchedHistory(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false);

	
	HTTPPacket::HTTPResponsePacket GetExplored(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false);

	

	
	/*****************内部调用接口**************/
	/*!
	验证用户登录是否有效，并且未被封禁
	参数：request | 当前用户的请求报文
	返回：符合条件为true, 否则为false
	异常：执行SQL语句时如遇错误会抛出sql::SQLException
	*/
	bool CheckUserToken(HTTPPacket::HTTPRequestPacket& request) noexcept(false);

	
	/*!
	根据用户名获取其uuid。
	异常：可能会抛出sql::SQLException
	*/
	std::string GetUUIDByUsername(std::string uuid) noexcept(false);

	/*!
	根据用户uuid获取其头像
	异常：可能会抛出sql::SQLException
	*/
	std::string GetUserAvatar(std::string uuid) noexcept(false);

	/*!
	根据用户uuid获取其用户名
	异常：可能会抛出sql::SQLException
	*/
	std::string GetUsernameByUUID(std::string username) noexcept(false);

	/*!
	查询指定文章是否存在。
	异常：可能会抛出sql::SQLException
	*/
	bool IsArticleExist(std::string aritcleId) noexcept(false);
};