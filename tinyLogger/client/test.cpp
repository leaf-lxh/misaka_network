#include <iostream>
#include "tinyLogger.h"

#include <ctime>

int main()
{
	TinyLoggerClient logger("xhttpd", "testkey", {"127.0.0.1", 7200});
	time_t startTime = time(nullptr);

	try
	{
		TinyLoggerClient::Response r = logger.Send("test logger success");
		std::cout << r.code << std::endl;
		std::cout << r.server << std::endl;
	}
	catch (std::runtime_error e)
	{
		std::cout << e.what() << std::endl;
	}

	return 0;
}
