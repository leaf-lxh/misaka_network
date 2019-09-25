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
	/*!
	��ʼ��·�ɱ�
	*/
	void InitRouteTabel() noexcept;

	///MYSQL���������
	struct MySQLProperty
	{
		//ip:port����
		std::string host;
		std::string user;
		std::string password;
		sql::Driver* driverInstance;
		std::unique_ptr<sql::Connection> connection;
	}mysqlProperty;

	/*!
	����ʼ��MYSQL���ݿ����ӡ���������������ʹ��reconnect����
	�쳣����ʼ��ʧ�ܣ����޷��������ݿ⣬���ܴ���ȣ����׳�runtime_error
	*/
	void InitMysqlConnection() noexcept(false);

	///MySQL����ָ��İ�װ
	using PtrPreparedStatement = std::unique_ptr<sql::PreparedStatement>;
	using PtrResultSet = std::unique_ptr<sql::ResultSet>;

	/*!
	�������HTTP���Ĵ�����
	*/
	void HTTPPacketHandler(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept;


	HTTPPacket::HTTPResponsePacket Login(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false);
	HTTPPacket::HTTPResponsePacket Register(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false);
	HTTPPacket::HTTPResponsePacket GetUserInfo(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false);
};
