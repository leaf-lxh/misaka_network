#pragma once

#include <vector>
#include <string>
class ServerSetting
{
	ServerSetting();

private:
	struct HostAttributes
	{
		bool isHTTPS;
		short port;
		std::string sourceRootDirectory;
	};

	struct CoreAttributes
	{
		std::vector<HostAttributes> hosts;
	};

public:
	CoreAttributes settings;
};
/********************************************************

*********************************************************/

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
*********************************************************/