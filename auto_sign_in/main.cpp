#include <iostream>
#include <sstream>
#include "modules/MySQL_connect.h"

std::string config_path = "./config/";
std::string log_path = "./log/";


bool MySQLTestLogin()
{
	std::string username = "";
	std::string password = "";

	std::cout << "程序开始运行，你需要做一些准备工作" << std::endl;
	std::cout << "首先需要进行数据库的一些配置" << std::endl;

	std::cout << "使用的用户为：";
	getline(std::cin, username);
	std::cout << "该用户密码为：";
	getline(std::cin, password);

	std::cout << "测试能否成功登录mysql数据库" << std::endl;
	MySQLConnect mysql_test;
	if (!mysql_test.init(username, password))
	{
		std::cout << "登录失败，请检查用户名和密码是否正确" << std::endl << "程序已退出" << std::endl;
		return false;
	}

	std::cout << "测试登录成功" << std::endl;
	return true;
}

void GetStartSignInTime()
{
	std::stringstream ConvertStream;
	std::string RawInput = "";
	int value = 0;

	int hour = -1;
	int minute = -1;

	
	
	while (hour<0 || hour>59)
	{
		std::cout << "签到时间设置" << std::endl << "时[0-59]：";
		getline(std::cin,RawInput);

		ConvertStream << RawInput;
		ConvertStream >> hour;
	}

	while (minute<0 || minute>59)
	{
		std::cout <<  "分[0-59]：";
		getline(std::cin, RawInput);
		ConvertStream.clear();

		ConvertStream << RawInput;
		ConvertStream >> minute;
	}
}
int main()
{
	
	if (!MySQLTestLogin())
	{
		return 0;
	}
	
	GetStartSignInTime();
	
    return 0;
}