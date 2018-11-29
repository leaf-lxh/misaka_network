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

	void WriteLog(const std::string &path, const std::string &log);
	void ReadFile(const std::string &path, std::string &buffer);
	void WriteFile(const std::string &path, std::string &content, bool isReWrite);
	
};



