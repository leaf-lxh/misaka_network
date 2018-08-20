#include <mysql/mysql.h>
#include "MySQL_connect.h"

#include <string>

/************************************************************************************************************************
*测试连接MySQL-server，应在调用本类其他函数之前调用本函数
*参数：info|一个包含连接用的信息的结构
*返回：布尔值，无错误返回true。反之false，错误信息保存至lastErrorString
*************************************************************************************************************************/
bool MySQLConnect::TestConnect(MySQLInfo info)
{
	MYSQL sql;

	if (Connect(&sql, info, nullptr) == nullptr)
	{
		lastErrorString = mysql_error(&sql);
		return false;
	}

	_info = info;
	return true;
}

unsigned long long MySQLConnect::InsertData(const char * tableName, const char * columnName, const char * value)
{
	std::string queryString = "INSERT into ";
	queryString += tableName;
	queryString += columnName ;
	queryString += "VALUES ";
	queryString += value;

	return 0;
}

std::vector<std::vector<std::string>> MySQLConnect::SelectData(const char * column, const char * tableName, const char * condition)
{
	return std::vector<std::vector<std::string>>();
}

unsigned long long MySQLConnect::UpdateData(const char * tableName, const char * changes, const char * condition)
{
	
	return 0;
}

unsigned long long MySQLConnect::DeleteData(const char * tableName, const char * condition)
{
	return 0;
}



MYSQL* MySQLConnect::Connect(MYSQL *mysql, MySQLInfo info, const char * db)
{
	
	mysql_init(mysql);
	return mysql_real_connect(mysql, info.host.c_str(), info.username.c_str(), info.password.c_str(), db, info.port, NULL, NULL);

	
}

void MySQLConnect::Close(MYSQL * p)
{
	mysql_close(p);
}


MySQLConnect::MySQLConnect()
{
	mysql_library_init(0, NULL, NULL);
}

//本类的析构函数，用于释放mysql库占用的内存
MySQLConnect::~MySQLConnect()
{
	mysql_library_end();
}



