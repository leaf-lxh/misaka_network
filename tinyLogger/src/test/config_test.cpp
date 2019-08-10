#include "../conf.h"
#include <iostream>

using namespace std;

int main()
{
	SettingReader config("/etc/tinyLogger/facilities/0-default.conf");
	cout << config.config.size() << endl;
	return 0;
}

