#include "./interfaces/passport.h"
#include <iostream>
BlogSpacePassport server;

int main()
{
	try
	{
		server.Init();
		server.InitRouteTabel();
		server.StartListen();
		server.StartHandleRequest();
	}
	catch (std::runtime_error e)
	{
		std::cout << e.what() << std::endl;
	}
};
