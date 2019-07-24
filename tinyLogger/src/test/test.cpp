#include <iostream>

#include "../conf.h"
#include "../network.h"

int main()
{
	using namespace std;

	SettingReader conf("/etc/tinyLogger/setting.conf");
	cout << "Number of readed settings: " << conf.ReadConfiguration() << endl;

	for(auto setting : conf.config)
	{
		cout << '[' << setting.first  << ']' << '=' << '[' << setting.second << ']' << endl;
	}

	cout << "Starting server..." << endl;
	Network server(conf.config);
	try {
		cout << "listen FD: " << server.StartListen() << endl;
	}
	catch (std::runtime_error e) {
		cout << e.what() << endl;
	}
	cin.get();
	return 0;
}
