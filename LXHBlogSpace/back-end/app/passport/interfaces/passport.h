#include <tinyHttpd/tinyHttpd.h>

#include <cppconn/driver.h>

class BlogSpacePassport : public TinyHttpd
{
public:
	void ModuleInit() noexcept(false);


private:
	/*!
	初始化路由表
	*/
	void InitRouteTabel() noexcept;

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

	/*!
	本服务的HTTP报文处理器
	*/
	void HTTPPacketHandler(int clientfd, TinyHttpd::HTTPRequestPacket request) noexcept;


	TinyHttpd::HTTPResponsePacket Login(int clientfd, TinyHttpd::HTTPRequestPacket request) noexcept(false);
	TinyHttpd::HTTPResponsePacket Register(int clientfd, TinyHttpd::HTTPRequestPacket request) noexcept(false);
	TinyHttpd::HTTPResponsePacket GetUserInfo(int clientfd, TinyHttpd::HTTPRequestPacket request) noexcept(false);
};
