#include "FileIO.h"

#include <fstream>
#include <ctime>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

/******************************************************************************************
*To read file content and store it to a string buffer.
* @path: The path of the file that to be readed.
* @buffer: The string buffer that will store file content.
*return value: nothing
******************************************************************************************/
void FileIO::ReadFile(const std::string &path, std::string &buffer)
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

/******************************************************************************************
*Writting given content to a specific file.
* @path: The path of the file that to be writed.
* @content: The content that will be writed to the file.
* @isReWrite: If this paramter is true, then the file will be over writed with the content that given.
*             Othervise the content will be appended to the original file content.
*return value: nothing.
******************************************************************************************/
void FileIO::WriteFile(const std::string &path, std::string &content ,bool isReWrite)
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

/******************************************************************************************
*To check the directory whether exist or not.
* @path: The path that to be checked.
*return value: If the directory exists, return true. 
               Otherwise return false.
******************************************************************************************/
bool FileIO::PathCheck(const std::string & path)
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

/******************************************************************************************
*Create directory.
* @path: The path that want to create,
*return value: If the creating succeed,return true.
               Otherwise return false.
******************************************************************************************/
bool FileIO::PathCreate(const std::string & path)
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

/******************************************************************************************
*Writing log in the form of "time: log\n".
* @path: log path.
* @log: A string that contents log.
*return value: nothing.
******************************************************************************************/
void FileIO::WriteLog(const std::string &path, const std::string &log)
{
	time_t currentUnixTime = time(NULL);
	std::string output = asctime(localtime(&currentUnixTime));
	output = output.substr(0, output.length() - 1);//remove \0 that created by asctime()
	output += ": " + log + '\n';

	WriteFile(path, output,false);
	
}

