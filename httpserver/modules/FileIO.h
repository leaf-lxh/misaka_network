#include <string>

class FileIO
{
public:
	FileIO() = default;

	void WriteLog(const std::string &path, const std::string &log);
	void ReadFile(const std::string &path, std::string &buffer);
	void WriteFile(const std::string &path, std::string &content, bool isReWrite);
	/*************************************************************************
	path should be like the form of /...../
	*************************************************************************/
	bool PathCheck(const std::string &path);
	bool PathCreate(const std::string &path);
};


