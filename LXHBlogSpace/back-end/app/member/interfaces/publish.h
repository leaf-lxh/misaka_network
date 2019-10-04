#include <tinyHttpd/tinyHttpd.h>

#include <tinyHttpd/HTTPPacket.h>

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>

class BlogSpacePublish : public TinyHttpd
{
public:
	void ModuleInit() noexcept(false);


private:
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
	using PtrStatement = std::unique_ptr<sql::Statement>;
	using PtrResultSet = std::unique_ptr<sql::ResultSet>;

	/*!
	�������HTTP���Ĵ�����
	*/
	void HTTPPacketHandler(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept;

	/****************�ⲿ�����ӿ�***************/
	HTTPPacket::HTTPResponsePacket UploadImage(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false);


	/*****************�ڲ����ýӿ�**************/
	/*!
	��֤�û���¼�Ƿ���Ч������δ�����
	������request | ��ǰ�û���������
	���أ���������Ϊtrue, ����Ϊfalse
	�쳣��ִ��SQL���ʱ����������׳�sql::SQLException
	*/
	bool CheckUserToken(HTTPPacket::HTTPRequestPacket& request) noexcept(false);


};