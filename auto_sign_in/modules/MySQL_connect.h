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
	bool Connect(MySQLInfo info);
	
	/************************************************************************************************************************
	*关闭一个建立的连接
	*参数：无
	*返回：无
	*************************************************************************************************************************/
	void  CloseConnection();

	void Query(const std::string query);

	void UseDatabase(const std::string db);

	/************************************************************************************************************************
	*执行INSERT语句
	*参数：tableName             |目标表名
	*      columnName            |与value对应的列表头，可以为空字符串
	*      value                 |数据
	*返回：unsigned long long    |影响的行数，如果执行失败则返回ULLONG_MAX
	*************************************************************************************************************************/
	unsigned long long InsertData(const std::string tableName, const std::string columnName, const std::string value);

	std::vector<std::vector<const std::string>> SelectData(const std::string column, const std::string tableName, const std::string condition);

	unsigned long long UpdateData(const std::string tableName, const std::string changes, const std::string condition);

	unsigned long long DeleteData(const std::string tableName, const std::string condition);


	
private:
	MYSQL _HMYSQL;
};


