#include <mysql/mysql.h>
#include "MySQL_connect.h"

#include <string>

//本类的构造函数，用于初始化mysqllib
MySQLConnect::MySQLConnect()
{
	mysql_library_init(0, NULL, NULL);
}


//本类的析构函数，用于释放mysql库占用的内存
MySQLConnect::~MySQLConnect()
{
	mysql_library_end();
}


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
*执行一个MySQL语句
*参数：query|欲执行的MySQL语句
*返回：bool |无错误为true,反之为false,具体错误查看lastErrorString
*************************************************************************************************************************/
bool MySQLConnect::Query(const std::string query)
{
	if (mysql_query(&_HMYSQL, query.c_str()))
	{
		lastErrorString = mysql_error(&_HMYSQL);
		return false;
	}

	return true;
}


/************************************************************************************************************************
*执行INSERT语句
*参数：tableName             |目标表名
*      columnName            |与value对应的列表头，可以为空字符串
*      value                 |数据
*返回：unsigned long long    |影响的行数，如果执行失败则返回(unsigned long long)-1
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


/************************************************************************************************************************
*执行SELECT语句
*参数：column                |所限定的列
*      tableName             |目标表名
*      condition             |条件
*      buffer                |用于存储结果
*返回：unsigned long long    |查询所返回结果的行数，若发生错误返回(unsigned long long)-1
*************************************************************************************************************************/
unsigned long long MySQLConnect::SelectData(const std::string column, const std::string tableName, const std::string condition, std::vector<std::vector<const std::string>> &buffer)
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
	
	if (mysql_query(&_HMYSQL, queryString.c_str()))
	{
		lastErrorString = mysql_error(&_HMYSQL);
		return (unsigned long long) - 1;
	}

	MYSQL_RES* results = mysql_store_result(&_HMYSQL);
	unsigned long long rowCount = 0;
	if (results == nullptr)
	{
		
			rowCount = (unsigned long long) - 1;
			return rowCount;
	}
	
	rowCount = mysql_num_rows(results);
	if (rowCount != 0)
	{

		MYSQL_ROW row = mysql_fetch_row(results);
		if (row == nullptr)
		{
			lastErrorString = mysql_error(&_HMYSQL);
			return (unsigned long long) - 1;
		}

		unsigned long long columnCount = mysql_num_fields(results);
		buffer = {};
		std::vector<const std::string> singleRowBuf = {};
		do
		{
			for (unsigned long long columnIndex = 0; columnIndex < columnCount; columnIndex++)
			{
				singleRowBuf.push_back(row[columnIndex]);
			}
			buffer.push_back(singleRowBuf);

			singleRowBuf = {};
			row = mysql_fetch_row(results);
		} while (row != nullptr);
	}

	return rowCount;
}


/************************************************************************************************************************
*更新目标表的数据
*参数：tableName|目标表名
*      changes  |变动
*      condition|条件，可为空字符串
*返回：unsigned long long    |影响的行数，如果执行失败则返回(unsigned long long)-1
*************************************************************************************************************************/
unsigned long long MySQLConnect::UpdateData(const std::string tableName, const std::string changes, const std::string condition)
{
	std::string queryString = "UPDATE";
	queryString += ' ';
	queryString += tableName;
	queryString += ' ';
	queryString += "SET";
	queryString += ' ';
	queryString += changes;
	if (condition != "")
	{
		queryString += ' ';
		queryString += "WHERE";
		queryString += ' ';
		queryString += condition;
	}
	

	if (mysql_query(&_HMYSQL, queryString.c_str()))
	{
		lastErrorString = mysql_error(&_HMYSQL);
		return (unsigned long long) - 1;
	}

	return mysql_affected_rows(&_HMYSQL);
}


/************************************************************************************************************************
*删除目标表中的数据
*参数：tableName|目标表名
*      condition|条件，可为空字符串
*返回：unsigned long long    |影响的行数，如果执行失败则返回(unsigned long long)-1
*************************************************************************************************************************/
unsigned long long MySQLConnect::DeleteData(const std::string tableName, const std::string condition)
{
	std::string queryString = "DELETE FROM";
	queryString += ' ';
	queryString += tableName;
	if (condition != "")
	{
		queryString += ' ';
		queryString += condition;
	}
	
	if (mysql_query(&_HMYSQL, queryString.c_str()))
	{
		lastErrorString = mysql_error(&_HMYSQL);
		return (unsigned long long) - 1;
	}
	return mysql_affected_rows(&_HMYSQL);
}




