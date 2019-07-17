#include <iostream>

#include "../conf.h"

int main()
{
	using namespace std;

	SettingReader conf("/etc/tinyLogger/setting.conf");
	cout << "Number of readed settings: " << conf.ReadConfiguration() << endl;

	for(auto setting : conf.config)
	{
		cout << '[' << setting.first  << ']' << '=' << '[' << setting.second << ']' << endl;
	}

	return 0;
}
