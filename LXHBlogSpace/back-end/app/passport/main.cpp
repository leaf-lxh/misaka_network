#include "./interfaces/passport.h"

BlogSpacePassport server;

int main()
{
	try
	{
		server.Init();
		server.LoadMime();
		server.AddRoute("/api/v1/passport");
		server.StartListen();
		server.StartHandleRequest();
	}
	catch (std::runtime_error e)
	{
		std::cout << e.what() << sd::endl;
	}
};
