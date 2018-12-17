/******************************************************************************************
FileName: FileIO.h
Author: leaf-lxh
Description: 进行文件的读写、判断某个路径是否存在，进行某个路径的创建
******************************************************************************************/

#include <string>

/******************************************************************************************
*To check the directory whether exist or not. Not working for existed file.
* @path: The path that to be checked.
*return value: If the directory exists, return true.
Otherwise return false.
******************************************************************************************/
bool PathCheck(const std::string &path);

/******************************************************************************************
*Create directory. Make sure the path is vaild before calling this function
* @path: The path that want to create,
*return value: If the creating succeed,return true.
Otherwise return false.
******************************************************************************************/
bool PathCreate(const std::string &path);

class FileIO
{
public:
	FileIO() = default;


	/******************************************************************************************
	*Writing log in the form of "time: log\n".
	* @path: log path.
	* @log: A string that contents log.
	*return value: nothing.
	******************************************************************************************/
	bool WriteLog(const std::string path, const std::string &log);


	/******************************************************************************************
	*To read file content and store it to a string buffer.
	* @path: The path of the file that to be readed.
	* @buffer: The string buffer that will store file content.
	*return value: no
	******************************************************************************************/
	bool ReadFile(const std::string path, std::string &buffer);


	/******************************************************************************************
	*Writting given content to a specific file.
	* @path: The path of the file that to be writed.
	* @content: The content that will be writed to the file.
	* @isReWrite: If this paramter is true, then the file will be over writed with the content that given.
	*             Othervise the content will be appended to the original file content.
	*return value: nothing.
	******************************************************************************************/
	bool WriteFile(const std::string path, std::string &content, bool isOverWrite);
	
};



