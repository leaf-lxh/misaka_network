#include <iostream>
#include <sstream>
#include "modules/MySQL_connect.h"
#include "modules/FileIO.h"
#include "modules/auto_sign_in.h"

std::string config_path = "./config/";
std::string log_path = "./log/";

std::string mysql_host = "localhost";
unsigned int mysql_port = 2333;
std::string mysql_username = "";
std::string mysql_password = "";

//bool MySQLTestLogin()
//{
//
//
//	std::cout << "程序开始运行，你需要做一些准备工作" << std::endl;
//	std::cout << "首先需要进行数据库的一些配置" << std::endl;
//
//	std::cout << "使用的用户为：";
//	getline(std::cin, mysql_username);
//	std::cout << "该用户密码为：";
//	getline(std::cin, mysql_password);
//
//	std::cout << "测试能否成功登录mysql数据库" << std::endl;
//	MySQLConnect mysql_test;
//	if (!mysql_test.init(mysql_host,mysql_port,mysql_username, mysql_password))
//	{
//		std::cout << "登录失败，请检查用户名和密码是否正确" << std::endl << "程序已退出" << std::endl;
//		return false;
//	}
//
//	std::cout << "测试登录成功" << std::endl;
//	return true;
//}
//
//void GetStartSignInTime()
//{
//	std::stringstream ConvertStream;
//	std::string RawInput = "";
//	SignInFileIO fileio;
//
//	ConfigStruct cs = { -1,-1 };
//
//	std::cout << "是否需要设置签到时间[y/n]：" << std::unitbuf;
//	getline(std::cin, RawInput);
//	if (RawInput == "y" || RawInput == "Y")
//	{
//		while (cs.hour < 0 || cs.hour>59)
//		{
//			std::cout << "时[0-59]：" << std::unitbuf;
//			getline(std::cin, RawInput);
//			ConvertStream.clear();
//
//			ConvertStream << RawInput;
//			ConvertStream >> cs.hour;
//		}
//
//		while (cs.minute < 0 || cs.minute>59)
//		{
//			std::cout << "分[0-59]：" << std::unitbuf;
//			getline(std::cin, RawInput);
//			ConvertStream.clear();
//
//			ConvertStream << RawInput;
//			ConvertStream >> cs.minute;
//		}
//		fileio.WriteConfig(cs);
//		std::cout << "时间：" << cs.hour << "时" << cs.minute << "分" << std::endl;
//	}
//	else
//	{
//		fileio.ReadConfig(cs);
//		std::cout << "签到时间为：" << cs.hour << "时" << cs.minute << "分" << std::endl;
//	}
//
//}
int main()
{
	std::cout << "Your BDUSS:" << std::endl;
	std::string BDUSS;
	getline(std::cin, BDUSS);
	SignIn request;
	std::vector<BarInfo>info = request.GetUserILikeList(BDUSS);

	for (auto index : info)
	{
		std::cout << index.is_sign << std::endl;
	}
	
	return 0;
}