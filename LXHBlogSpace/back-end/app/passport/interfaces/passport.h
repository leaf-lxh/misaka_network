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

	struct EmailProperty
	{
		std::string server;
		std::string port;
		std::string sender;
		std::string auth;
	}emailProperty;

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
	HTTPPacket::HTTPResponsePacket Login(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false);
	HTTPPacket::HTTPResponsePacket Logout(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false);

	HTTPPacket::HTTPResponsePacket CheckUserExist(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false);
	HTTPPacket::HTTPResponsePacket CheckEmailExist(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false);
	HTTPPacket::HTTPResponsePacket Register(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false);
	HTTPPacket::HTTPResponsePacket SendEmailAuth(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false);
	HTTPPacket::HTTPResponsePacket GetUserInfo(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false);

	HTTPPacket::HTTPResponsePacket IsLogin(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false);

	HTTPPacket::HTTPResponsePacket UpdateUserDetails(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false);


	/*****************�ڲ����ýӿ�**************/
	/*!
	���ָ���û��Ƿ����
	������username | ָ�����û���
	���أ�����Ϊtrue, ������Ϊfalse
	�쳣����MySQL���ù����з����쳣ʱ�׳�sql::SQLException
	*/
	bool CheckUserExist(std::string username) noexcept(false);

	/*!
	���ָ���û��Ƿ����
	������email | ָ�����û���
	���أ�����Ϊtrue, ������Ϊfalse
	�쳣����MySQL���ù����з����쳣ʱ�׳�sql::SQLException
	*/
	bool CheckEmailExist(std::string email) noexcept(false);

	bool IsVaildUserName(std::string username) noexcept;

	bool IsVaildPassword(std::string password) noexcept;

	/*!
	��������ַ��ʽ�Ƿ���ϸ�ʽ�����API�ĵ����û�ע�Ჿ��
	���أ��Ϻ��淶Ϊtrue�����򷵻�false
	*/
	bool IsVaildEmailAddress(std::string email) noexcept;

	/*****************�ڲ����ýӿ�**************/
	/*!
	��֤�û���¼�Ƿ���Ч������δ�������
	������request | ��ǰ�û���������
	���أ���������Ϊtrue, ����Ϊfalse
	�쳣��ִ��SQL���ʱ����������׳�sql::SQLException
	*/
	bool CheckUserToken(HTTPPacket::HTTPRequestPacket& request) noexcept(false);
};
