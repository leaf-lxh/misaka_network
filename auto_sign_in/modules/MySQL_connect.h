#include <string>
#include <vector>
#include <sstream>

struct MySQLInfo
{
	std::string host;
	unsigned int port;
	std::string username;
	std::string password;
};

class MySQLConnect
{
public:

	MySQLConnect();
	~MySQLConnect();

	std::string lastErrorString;

	/************************************************************************************************************************
	*测试连接MySQL-server，应在调用本类其他函数之前调用本函数
	*参数：info|一个包含连接用的信息的结构
	*返回：布尔值，无错误返回true。反之false，错误信息查看lastErrorString
	*************************************************************************************************************************/
	bool TestConnect(MySQLInfo info);
	
	void Query(const char* query);

	void UseDatabase(const char* db);


	unsigned long long InsertData(const char* tableName, const char* columnName, const char* value);

	std::vector<std::vector<std::string>> SelectData(const char* column, const char* tableName, const char* condition);

	unsigned long long UpdateData(const char* tableName, const char* changes, const char* condition);

	unsigned long long DeleteData(const char* tableName, const char* condition);


	
private:
	MySQLInfo _info;

	MYSQL* Connect(MYSQL *mysql, MySQLInfo info, const char* db);
	void  Close(MYSQL* p);



};


