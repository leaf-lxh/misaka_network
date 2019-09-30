#include "./interfaces/content.h"
#include <iostream>
BlogSpaceContent server;

#include <unistd.h>

int main()
{
	try
	{

		server.ModuleInit();
		server.StartListen();
		//daemon(0, 0);
		server.StartHandleRequest();

	}
	catch (std::runtime_error e)
	{
		std::cout << e.what() << std::endl;
	}
};
