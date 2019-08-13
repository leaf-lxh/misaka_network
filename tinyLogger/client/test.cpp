#include <iostream>
#include "tinyLogger.h"

#include <ctime>

int main()
{
	TinyLoggerClient logger("xhttpd", "testkey", {"127.0.0.1", 7200});
	time_t startTime = time(nullptr);

	try { std::cout << logger.Send("test logger success").msg << std::endl; }
	catch (std::runtime_error e) { std::cout << e.what() << std::endl; }

	return 0;
}
