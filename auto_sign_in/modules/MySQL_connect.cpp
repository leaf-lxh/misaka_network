#include <mysql/mysql.h>
#include "MySQL_connect.h"

#include <string>

/************************************************************************************************************************
*与MySQL-server建立连接，应在调用本类其他函数之前调用本函数
*参数：info |一个包含连接用信息的MySQLInfo结构
*返回：布尔值，无错误返回true。反之false，错误信息保存至lastErrorString
*************************************************************************************************************************/
bool MySQLConnect::Connect(MySQLInfo info)
{
	
	mysql_init(&_HMYSQL);
	if (mysql_real_connect(&_HMYSQL, info.host.c_str(), info.username.c_str(), info.password.c_str(), NULL, info.port, NULL, NULL) == nullptr)
	{
		lastErrorString = mysql_error(&_HMYSQL);
		return false;
	}
	
	return true;
}


/************************************************************************************************************************
*关闭一个建立的连接
*参数：无
*返回：无
*************************************************************************************************************************/
void MySQLConnect::CloseConnection()
{
	mysql_close(&_HMYSQL);
}


/************************************************************************************************************************
*执行INSERT语句
*参数：tableName             |目标表名
*      columnName            |与value对应的列表头，可以为空字符串
*      value                 |数据
*返回：unsigned long long    |影响的行数，如果执行失败则返回ULLONG_MAX
*************************************************************************************************************************/
unsigned long long MySQLConnect::InsertData(const std::string tableName, const std::string columnName, const std::string value)
{
	std::string queryString = "INSERT into";
	queryString += ' ';
	queryString += tableName;
	queryString += ' ';
	queryString += columnName ;
	queryString += ' ';
	queryString += "VALUES";
	queryString += ' ';
	queryString += value;

	if (mysql_query(&_HMYSQL, queryString.c_str()))
	{
		lastErrorString = mysql_error(&_HMYSQL);
		return (unsigned long long) - 1;
	}

	return mysql_affected_rows(&_HMYSQL);
}

std::vector<std::vector<const std::string>> MySQLConnect::SelectData(const std::string column, const std::string tableName, const std::string condition)
{
	std::string queryString = "SELECT";
	queryString += ' ';
	queryString += column;
	queryString += ' ';
	queryString += "FROM";
	queryString += ' ';
	queryString += tableName;
	if (condition != "")
	{
		queryString += ' ';
		queryString += "WHERE";
		queryString += ' ';
		queryString += condition;
	}
	
	
	return std::vector<std::vector<const std::string>>();
}

unsigned long long MySQLConnect::UpdateData(const std::string tableName, const std::string changes, const std::string condition)
{
	
	return 0;
}

unsigned long long MySQLConnect::DeleteData(const std::string tableName, const std::string condition)
{
	return 0;
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



