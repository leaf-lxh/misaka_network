#include "tinyHttpd.h"
#include <iostream>

int main(int argc, char **argv)
{
	using namespace std;
	TinyHttpd server;
	try
	{
		server.Init();
		server.DumpProperty();
//		server.StartListen();
	}
	catch (std::runtime_error e)
	{
		cout << "Server init error: " << e.what() << endl;
	}
	
	server.StartHandleRequest();

	return 0;
}