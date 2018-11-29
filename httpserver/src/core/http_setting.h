#pragma once

#include <vector>
#include <string>

class ServerSetting
{
public:
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
		int threadPerHost;
		std::vector<HostAttributes> hosts;
	};

public:
	CoreAttributes settings;
};
