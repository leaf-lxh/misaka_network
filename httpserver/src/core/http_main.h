#pragma once
#include <string>



class ServerCore {
public:
	ServerCore();

	/**********************************************
	*
	*
	**********************************************/
	~ServerCore();
public:
	struct ServerStatus {
		std::string startTime;
	};

	int RequestServerStart();

	int RequestServerClose(std::string reason);
private:


};