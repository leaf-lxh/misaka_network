#pragma once
#include <string>


struct ConfigStruct
{
	int hour;
	int minute;
};
class SignInFileIO
{
public:
	std::string config_path = "/etc/lxh/";
	std::string log_path = "/var/log/lxh/";

	std::string config_name = "lxh.config";
	std::string log_name = "lxh.log"; 

	bool ReadConfig(ConfigStruct &cs);
	bool WriteConfig(ConfigStruct &cs);

	bool WriteLog(const std::string &log);

};

