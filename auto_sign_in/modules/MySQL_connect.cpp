#include <mysql/mysql.h>
#include "MySQL_connect.h"


bool MySQLConnect::init(std::string host, unsigned int port, std::string &username, std::string &password)
{
	mysql_library_init(0,NULL,NULL);
	MYSQL mysql;
	mysql_init(&mysql);

	if(mysql_real_connect(&mysql,host.c_str(),username.c_str(),password.c_str(),NULL,port,NULL, 0) == NULL)
	{
		return false;
	}

	_MySQLInfo.host = host;
	_MySQLInfo.port = port;
	_MySQLInfo.username = username;
	_MySQLInfo.password = password;	
	return true;
}

void MySQLConnect::destory()
{
	mysql_library_end();
}

//bool MySQLConnect::

