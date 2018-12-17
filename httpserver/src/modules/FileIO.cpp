#include "FileIO.h"

#include <fstream>
#include <ctime>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>


bool FileIO::ReadFile(const std::string path, std::string &buffer)
{
	if (!PathCheck(path))
	{
		if (!PathCreate(path))
		{
			return;
		}

	}
	using std::fstream;

	fstream file(path.c_str(), fstream::in);

	std::string temp;

	buffer = "";
	bool isFirstLine = true;
	while (getline(file, temp))
	{
		if (isFirstLine)
		{
			isFirstLine = false;
		}
		else
		{
			buffer += '\n';
		}
		buffer += temp;
	}

	
	file.close();
}


bool FileIO::WriteFile(const std::string path, std::string &content ,bool isReWrite)
{
	if (!PathCheck(path))
	{
		if (!PathCreate(path))
		{
			return;
		}
	}

	using std::fstream;
	fstream file;
	if (isReWrite == true)
	{
		file.open(path.c_str(), fstream::out);
	}
	else
	{
		file.open(path.c_str(), fstream::app);
	}

	file << content;

	file.close();
}

bool FileIO::WriteLog(const std::string path, const std::string &log)
{
	time_t currentUnixTime = time(NULL);
	std::string output = asctime(localtime(&currentUnixTime));
	output = output.substr(0, output.length() - 1);//remove \0 that created by asctime()
	output += ": " + log + '\n';

	WriteFile(path, output, false);

}


bool PathCheck(const std::string & path)
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


bool PathCreate(const std::string & path)
{
	std::string currentPath;
	size_t former = 0;
	size_t latter = path.rfind('/') + 1;

	do
	{
		former = path.find('/',former);
		if (former == currentPath.npos)
		{
			break;
		}
		former += 1;
		currentPath = path.substr(0, former);
		if (access(currentPath.c_str(), F_OK))
		{
			mkdir(currentPath.c_str(), 744);
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




