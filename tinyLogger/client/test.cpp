#include <iostream>
#include "tinyLogger.h"

#include <ctime>

int main()
{
	TinyLoggerClient logger("xhttpd", "testkey", {"127.0.0.1", 7200});
	time_t startTime = time(nullptr);
	std::cout << "Time: " << startTime << std::endl;
	std::cout << "Start testing..." << std::endl;

	for (int i = 0; i < 10000; ++i)
	{
		try { logger.Send("test logger success"); }
		catch (std::runtime_error e) { std::cout << e.what() << std::endl; }
	}
	time_t endTime = time(nullptr);
	std::cout << "Testing Stop" << std::endl;
	std::cout << "Period: " << endTime - startTime << std::endl;

	return 0;
}
