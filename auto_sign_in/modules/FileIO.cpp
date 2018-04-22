#include <sys/types.h>  
#include <sys/stat.h> 
#include <unistd.h>  
#include "FileIO.h"
#include <sstream>
#include <fstream>

bool SignInFileIO::ReadConfig(ConfigStruct & cs)
{
	//��ʼ�����ýṹ
	cs.hour = -1;
	cs.minute = -1;

	//�ж�Ŀ¼�Ƿ����,�������򴴽������������򷵻�false
	if (!access(config_path.c_str(), F_OK))
	{
		if (mkdir(config_path.c_str(), 644))
		{
			return false;
		}
	}

	//׼����ȡ�ļ�
	std::fstream file;
	std::string FileContent = "";
	file.open(config_path, std::fstream::app);//ʹ��appendģʽ������ļ�������Ҳ���Դ���

	std::stringstream ss;
	if (!file.good())
	{
		return false;
	}

	while (getline(file, FileContent))
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
	}

	if (cs.hour<0 || cs.hour>23)
	{
		cs.hour = 2;
	}

	if (cs.minute < 0 || cs.minute>59)
	{
		cs.minute = 0;
	}
	WriteConfig(cs);
	return false;
}

bool SignInFileIO::WriteConfig(ConfigStruct & cs)
{
	return false;
}

bool SignInFileIO::WriteLog(const std::string & log)
{
	return false;
}
