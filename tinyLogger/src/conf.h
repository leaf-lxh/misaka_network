#include <string>
#include <map>
#include <stdexcept>

class SettingReader
{
public:
	const std::string path;

	std::map<std::string, std::string> config;

	SettingReader(std::string settingPath="/etc/tinyLogger/setting.conf") : path(settingPath) {}

	unsigned int ReadConfiguration();

private:
	std::string strip(const std::string str);
};
