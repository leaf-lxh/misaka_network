#include <iostream>
#include <sstream>
#include "modules/FileIO.h"
#include "modules/auto_sign_in.h"



int main()
{
	std::cout << "-------------------------初始化-------------------------" << std::endl;
	MySQLInfo info;
	std::cout << "数据库账号[root]";
	getline(std::cin, info.username);
	if (info.username == "")
	{
		info.username = "root";
	}

	std::cout << "密码:";
	getline(std::cin, info.username);

	std::cout << "端口[3306]: ";
	std::string port;
	getline(std::cin, port);
	if (port == "")
	{
		info.port = 3306;
	}
	else
	{
		std::stringstream ss;
		ss << port;
		ss >> info.port;
	}

	std::cout << "主机[127.0.0.1]";
	getline(std::cin, info.host);
	
	std::cout << "开始监视，程序切换至后台运行" << std::endl;
	SignTask task(info);
	task.StartWatching();

	return 0;
}