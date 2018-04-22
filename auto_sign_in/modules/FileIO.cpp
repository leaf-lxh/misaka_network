#include <sys/types.h>  
#include <sys/stat.h> 
#include <unistd.h>  
#include "FileIO.h"
#include <sstream>
#include <fstream>

bool SignInFileIO::ReadConfig(ConfigStruct & cs)
{
	//初始化配置结构
	cs.hour = -1;
	cs.minute = -1;

	//判断目录是否存在,不存在则创建，创建不了则返回false
	if (!access(config_path.c_str(), F_OK))
	{
		if (mkdir(config_path.c_str(), 644))
		{
			return false;
		}
	}

	//准备读取文件
	std::fstream file;
	std::string FileContent = "";
	file.open(config_path, std::fstream::app);//使用append模式，如果文件不存在也可以创建

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
