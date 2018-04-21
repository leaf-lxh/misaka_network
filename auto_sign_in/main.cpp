#include <iostream>
#include <sstream>
#include "modules/MySQL_connect.h"

std::string config_path = "./config/";
std::string log_path = "./log/";


bool MySQLTestLogin()
{
	std::string username = "";
	std::string password = "";

	std::cout << "����ʼ���У�����Ҫ��һЩ׼������" << std::endl;
	std::cout << "������Ҫ�������ݿ��һЩ����" << std::endl;

	std::cout << "ʹ�õ��û�Ϊ��";
	getline(std::cin, username);
	std::cout << "���û�����Ϊ��";
	getline(std::cin, password);

	std::cout << "�����ܷ�ɹ���¼mysql���ݿ�" << std::endl;
	MySQLConnect mysql_test;
	if (!mysql_test.init(username, password))
	{
		std::cout << "��¼ʧ�ܣ������û����������Ƿ���ȷ" << std::endl << "�������˳�" << std::endl;
		return false;
	}

	std::cout << "���Ե�¼�ɹ�" << std::endl;
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
		std::cout << "ǩ��ʱ������" << std::endl << "ʱ[0-59]��";
		getline(std::cin,RawInput);

		ConvertStream << RawInput;
		ConvertStream >> hour;
	}

	while (minute<0 || minute>59)
	{
		std::cout <<  "��[0-59]��";
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