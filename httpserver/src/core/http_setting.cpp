#include "http_setting.h"

#include <regex>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include "../modules/FileIO.h"
///etc/httpservre/setting.conf
/********************************************************
ThreadPerHost=10
EnableMultiHosts=false
********************************************************/

///etc/httpservre/hosts.conf
/********************************************************
<!--This is an example configuration file-->
<host>
<usingHTTPS>false</usingHTTPS>
<port>80</port>
<sourceRootDirectory>/var/www/host1</sourceRootDirectory>
</host>

<host>
<usingHTTPS>true</usingHTTPS>
<port>443</port>
<sourceRootDirectory>/var/www/host2</sourceRootDirectory>
</host>
********************************************************/

ServerSetting::ServerSetting()
{
	settings.threadPerHost = 10;
	
	std::fstream file("/etc/httpservre/setting.conf",std::fstream::in);
	if (!file.good())
	{
		std::runtime_error("Can't open setting file \"/etc/httpservre/setting.conf\".");
	}

	std::string line;
	std::string config;
	while (getline(file,line))
	{
		config += line + "\n";
	}
	std::stringstream ss;

	std::regex format("^ThreadPerHost=(.+?)");
	std::smatch result;
	if (std::regex_search(config, result,format))
	{
		ss << result[1].str();
		ss >> this->settings.threadPerHost;
		ss.clear();
	}

}