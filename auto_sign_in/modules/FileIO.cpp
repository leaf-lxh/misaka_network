#include <sys/types.h>  
#include <sys/stat.h> 
#include <unistd.h>  
#include "FileIO.h"
#include <sstream>
#include <fstream>
#include <ctime>

#include <iostream>


bool SignInFileIO::ReadConfig(ConfigStruct & cs)
{
	
	cs.hour = -1;
	cs.minute = -1;

	if (!access(config_path.c_str(), F_OK))
	{
		if (!mkdir(config_path.c_str(), 644))
		{
			return false;
		}
	}

	std::fstream file;
	std::string FileContent = "";
	file.open(config_path+config_name, std::fstream::in);

	
	if (!file.good())
	{
		return false;
	}

	std::stringstream ss;
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
		WriteLog("Config.hour setting error. Changed to defult value 2");
		cs.hour = 2;
	}

	if (cs.minute < 0 || cs.minute>59)
	{
		WriteLog("Config.minute setting error. Changed to defult value 0");
		cs.minute = 0;
	}
	
	file.close();
	WriteConfig(cs);
	return true;
}

bool SignInFileIO::WriteConfig(ConfigStruct & cs)
{
	if (!access(config_path.c_str(), F_OK))
	{
		if (!mkdir(config_path.c_str(), 644))
		{
			return false;
		}
	}

	std::fstream file;
	std::string FileContent = "";
	file.open(config_path + config_name, std::fstream::out);

	if (!file.good())
	{
		return false;
	}

	file << "hour=" << cs.hour << std::endl;
	file<<"minute=" << cs.minute << std::flush;

	file.close();

	return true;
}

bool SignInFileIO::WriteLog(const std::string & log)
{
	

	if (!access(log_path.c_str(), F_OK))
	{
		if (!mkdir(log_path.c_str(), 644))
		{
			return false;
		}
	}

	std::fstream file;

	file.open(log_path + log_name, std::fstream::out | std::fstream::app);

	if (!file.good())
	{
		return false;
	}

	time_t CurrentTime = time(NULL);

	std::string TimeString = ctime(&CurrentTime);
	TimeString =  TimeString.substr(0, TimeString.length()-1);

	std::cout << TimeString << ": " << log << std::endl;
	file << TimeString << ": " << log << std::endl;

	file.close();

	return true;
}
