#include "xhttp.h"

namespace xhttp
{
	XHTTPServer::XHTTPServer(std::string settingPath, std::string unixSocketRootPath)
	{
		mSettingPath = settingPath;
		mUnixSocketRootPath = unixSocketRootPath;
	}

	int XHTTPServer::InitDefaultServer()
	{

		return 0;
	}
}