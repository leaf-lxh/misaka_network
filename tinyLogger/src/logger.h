#include "network.h"


class TinyLogger : public Network
{
public:
	TinyLogger();

	/*!
	Get log facilitites from /etc/tinyLogger/facilities
	params: none
	return: The number of facilities that aquired. If error occured, return -1
	*/
	int AquireFacilities();
	
private:

	/*facilities, use app_name as the key of each pair, the value is the properties of the facility*/
	std::map<std::string, std::map<std::string, std::string> > services;

	std::string EventHandler(std::string &buffer) override;
	
	std::map<std::string, std::string> ProtocolParser(const std::string headersBytes);

	bool WriteLog(std::string appName, std::string content);

	std::string CreateResponseMsg(std::string msg);
};

