#include "../logger.h"

#include <iostream>

using namespace std;
int main()
{
	TinyLogger logger;
	cout << "Facilities amount:" << logger.AquireFacilities() << endl;

	for (auto service : logger.services)
	{
		cout << "Service name: " << service.first << endl;
		for (auto config : service.second)
		{
			cout << "	" << config.first << ": " << config.second << endl;
		}
	}

	return 0;
}
