#include <tinyHttpd/tinyHttpd.h>

#include <cppconn/driver.h>

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

	/*!
	�������HTTP���Ĵ�����
	*/
	void HTTPPacketHandler(int clientfd, TinyHttpd::HTTPRequestPacket request) noexcept;


	TinyHttpd::HTTPResponsePacket Login(int clientfd, TinyHttpd::HTTPRequestPacket request) noexcept(false);
	TinyHttpd::HTTPResponsePacket Register(int clientfd, TinyHttpd::HTTPRequestPacket request) noexcept(false);
	TinyHttpd::HTTPResponsePacket GetUserInfo(int clientfd, TinyHttpd::HTTPRequestPacket request) noexcept(false);
};
