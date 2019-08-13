#include <pthread.h>

#include "tinyLogger.h"
#include <iostream>
#include <unistd.h>

using namespace std;

static void* Send(void *arg)
{
	for (int i = 0; i < 10000; ++i)
	{
		TinyLoggerClient* logger = new TinyLoggerClient("access-default", "e5ee161d14f7ba46bb007dc09c5bbe2ca5dcf70d", {"192.168.247.25", 7200});
		try 
		{
			cout << logger->Send((char*)arg).server << endl;
		}
		catch (std::runtime_error e)
		{
			cout << e.what() << endl;
			sleep(1);
		}

	}
	pthread_exit(0);
}

int main()
{ 
	pthread_t tid1, tid2;
	pthread_create(&tid1, NULL, Send, (void *)"AAAA");
	pthread_create(&tid2, NULL, Send, (void *)"BBBB");

	pthread_join(tid1, 0);
	pthread_join(tid2, 0);
	return 0;
}
