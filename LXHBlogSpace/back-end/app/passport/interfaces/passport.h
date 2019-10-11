#include <tinyHttpd/tinyHttpd.h>

#include <tinyHttpd/HTTPPacket.h>

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>

class BlogSpacePassport : public TinyHttpd
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

	struct EmailProperty
	{
		std::string server;
		std::string port;
		std::string sender;
		std::string auth;
	}emailProperty;

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
	HTTPPacket::HTTPResponsePacket Login(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false);
	HTTPPacket::HTTPResponsePacket Logout(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false);

	HTTPPacket::HTTPResponsePacket CheckUserExist(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false);
	HTTPPacket::HTTPResponsePacket CheckEmailExist(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false);
	HTTPPacket::HTTPResponsePacket Register(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false);
	HTTPPacket::HTTPResponsePacket SendEmailAuth(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false);
	HTTPPacket::HTTPResponsePacket GetUserInfo(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false);

	HTTPPacket::HTTPResponsePacket IsLogin(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false);

	HTTPPacket::HTTPResponsePacket UpdateUserDetails(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false);


	/*****************内部调用接口**************/
	/*!
	检查指定用户是否存在
	参数：username | 指定的用户名
	返回：存在为true, 不存在为false
	异常：在MySQL调用过程中发生异常时抛出sql::SQLException
	*/
	bool CheckUserExist(std::string username) noexcept(false);

	/*!
	检查指定用户是否存在
	参数：email | 指定的用户名
	返回：存在为true, 不存在为false
	异常：在MySQL调用过程中发生异常时抛出sql::SQLException
	*/
	bool CheckEmailExist(std::string email) noexcept(false);

	bool IsVaildUserName(std::string username) noexcept;

	bool IsVaildPassword(std::string password) noexcept;

	/*!
	检查邮箱地址格式是否符合格式，详见API文档中用户注册部分
	返回：合乎规范为true，否则返回false
	*/
	bool IsVaildEmailAddress(std::string email) noexcept;

	/*****************内部调用接口**************/
	/*!
	验证用户登录是否有效，并且未被封禁。
	参数：request | 当前用户的请求报文
	返回：符合条件为true, 否则为false
	异常：执行SQL语句时如遇错误会抛出sql::SQLException
	*/
	bool CheckUserToken(HTTPPacket::HTTPRequestPacket& request) noexcept(false);
};
