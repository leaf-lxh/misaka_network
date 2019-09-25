#include "./interfaces/passport.h"
#include <iostream>
BlogSpacePassport server;

#include <unistd.h>

int main()
{
	try
	{
		server.ModuleInit();
		server.StartListen();
		server.StartHandleRequest();
		//daemon(0, 0);
	}
	catch (std::runtime_error e)
	{
		std::cout << e.what() << std::endl;
	}
};
