#include "tinyHttpd.h"
#include <iostream>
#include <signal.h>
TinyHttpd server;
void sigterm(int signal)
{
	server.~TinyHttpd();
	exit(0);
}

int main(int argc, char **argv)
{
	signal(SIGTERM, sigterm);
	using namespace std;
	try
	{
		server.Init();
		server.LoadMIME();
		server.StartListen();
		server.DumpProperty();
	}
	catch (std::runtime_error e)
	{
		cout << "Server init error: " << e.what() << endl;
	}
	
	cout << "Server start running." << endl;
	server.StartHandleRequest();

	return 0;
}
