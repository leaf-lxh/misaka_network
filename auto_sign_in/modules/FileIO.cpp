#include <sys/types.h>  
#include <sys/stat.h> 
#include <unistd.h>  
#include "FileIO.h"

#include <sstream>
#include <fstream>
#include <ctime>
#include <regex>

/************************************************************************************************************************
*�������ļ���ȡ����
*��������
*���أ�bool ��ȡ�ɹ�����true����֮ false
*************************************************************************************************************************/
bool SignInFileIO::ReadConfig()
{
	
	if (!PathCheck(_configFilePath))
	{
		if (!PathCreate(_configFilePath, 644))
		{
			return false;
		}
	}

	std::fstream file;
	std::string FileContent = "";
	file.open(_configFilePath, std::fstream::in);

	
	if (!file.good())
	{
		file.close();
		file.open(_configFilePath, std::fstream::out);
		if (!file.good())
		{
			return false;
		}

		file << "#This is the configuration file for auto_sign_in.(https://github.com/leaf-lxh/misaka_network/tree/master/auto_sign_in)" << std::endl;
		file << "The configuration format should be like this: Name=Option#Comment" << std::endl;
		file << "Each separated with a newline character." << std::endl;
		file << "hour=1" << std::endl;
		file << "minutes=0" << std::endl;
		file << "logPath=\"/var/log/auto_sign_in/log.txt\"" << std::endl;

		config.hour = 1;
		config.minute = 0;
		config.log_path = "/var/log/auto_sign_in/log.txt";

		file.close();
		return true;
	}

	std::stringstream ss;

	/*while (getline(file, FileContent))
	{
		
		if (FileContent.find("hour=")!=std::string::npos)
		{
			ss.clear();
			ss << FileContent.substr(FileContent.find("=") + 1, FileContent.length());
			ss >> cs.hour;
		}
		
		if (FileContent.find("minute=") != std::string::npos)
		{
			ss.clear();
			ss << FileContent.substr(FileContent.find("=") + 1, FileContent.length());
			ss >> cs.minute;
		}
	}*/
	//18-9-24 ������ע�ʹ����ع�
	while (getline(file, FileContent))
	{
		std::regex format("(.*?)=(.*?)#.*?\n|(.*?)=(.*?)\n");
		std::smatch result;
		if (!std::regex_search(FileContent, result, format))
		{
			continue;
		}
		std::string configName;
		std::string configContent;
		if (result[1] == false)
		{
			configName = result[3];
			configContent = result[4];
		}
		else
		{
			configName = result[1];
			configContent = result[2];
		}

		if (configName == "hour")
		{
			ss << configContent;
			ss >> config.hour;
			ss.clear();
		}
		else if (configName == "minutes")
		{
			ss << configContent;
			ss >> config.minute;
			ss.clear();
		}
		else if (configName == "logPath")
		{
			config.log_path = configContent;
		}

	}

	if (config.hour<0 || config.hour>23)
	{
		WriteLog("Config.hour setting error. Changed to defult value 1");
		config.hour = 1;
	}

	if (config.minute < 0 || config.minute>59)
	{
		WriteLog("Config.minute setting error. Changed to defult value 0");
		config.minute = 0;
	}
	
	file.close();
	return true;
}

/************************************************************************************************************************
*��log�ļ��ײ�׷������, log�ļ�·��ȡ�������ļ�
*������log        | ��׷����log�ļ�������
*���أ�bool ���д��ɹ�����true����֮ false
*************************************************************************************************************************/
bool SignInFileIO::WriteLog(const std::string log)
{
	if (!PathCheck(config.log_path))
	{
		if (!PathCreate(config.log_path,744))
		{
			return false;
		}
	}

	std::fstream file;

	file.open(config.log_path, std::fstream::out | std::fstream::app);

	if (!file.good())
	{
		return false;
	}

	time_t CurrentTime = time(NULL);

	std::string TimeString = ctime(&CurrentTime);
	TimeString =  TimeString.substr(0, TimeString.length()-1);

	file << TimeString << ": " << log << std::endl;

	file.close();

	return true;
}

/************************************************************************************************************************
*��ѯ·���Ƿ����
*������path        | ����ѯ��·��
*���أ�bool ������ڣ�����true ��֮ false
*************************************************************************************************************************/
bool SignInFileIO::PathCheck(const std::string path)
{
	if (!access(path.c_str(), F_OK))
	{
		return true;
	}
	else
	{
		return false;
	}
}

/************************************************************************************************************************
*�Ը�Ŀ¼Ϊ��ǰĿ¼������Ŀ¼��֧�ִ������Ŀ¼
*������path        | ��������·����·������ Ŀ¼/�ļ�������ֻ����Ŀ¼���������ļ�
           mode      | Ŀ¼Ȩ��
*���أ�bool �����ɹ�����true ��֮ false
*************************************************************************************************************************/
bool SignInFileIO::PathCreate(const std::string  path, unsigned int mode)
{
	std::string currentPath;
	size_t former = 0;
	size_t latter = path.rfind('/') + 1;

	do
	{
		former = path.find('/', former);
		if (former == currentPath.npos)
		{
			break;
		}
		former += 1;
		currentPath = path.substr(0, former);
		if (access(currentPath.c_str(), F_OK))
		{
			mkdir(currentPath.c_str(), mode);
		}
	} while (former != latter);

	if (access(path.c_str(), F_OK))
	{
		return true;
	}
	else
	{
		return false;
	}
}