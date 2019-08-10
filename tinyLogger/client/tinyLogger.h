#include <string>
#include <map>
#include <utility>
#include <stdexcept>

#define TINYLOGGER_CLIENT_VERSION "1.0"

class TinyLoggerClient
{
public:
	struct Response
	{
		int code;
		std::string msg;
	};

	std::string appName;
	std::string authKey;
	std::pair<std::string, int> remote;

	TinyLoggerClient(std::string appName, std::string authKey, std::pair<std::string, unsigned short> remote)
	: appName(appName), authKey(authKey), remote(remote) { connectionFD = -1; }

	~TinyLoggerClient();
	/*!
	Send log to remote server.
	exception: std::runtime_error if error occured, like failing to conncect to server.
	param: timefmt | The time format. Default is YYYY-MM-DD hh:mm::ss (%F %T)
	param: logText | The text that  will be sended to server.
	return 
	*/
	Response Send(std::string logText, std::string timefmt = std::string("%F %T "));
private:
	int connectionFD;

	/*!
	connect to remote server
	return: If success, return true, and set connectionFD. Otherwise return false.
	*/
	bool Connect();

	std::map<std::string, std::string> ProtocolParser(const std::string headersBytes);
};
