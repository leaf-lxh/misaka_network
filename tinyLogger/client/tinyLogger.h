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
		std::string server;
		std::string msg;
	};

	std::string appName;
	std::string authKey;
	std::pair<std::string, int> remote;

	TinyLoggerClient(std::string appName, std::string authKey, std::pair<std::string, unsigned short> remote)
	: appName(appName), authKey(authKey), remote(remote) { connectionfd = -1; }

	~TinyLoggerClient();

	bool Connect() noexcept(false);
	Response Send(std::string logText, std::string timefmt = std::string("%F %T ")) noexcept(false);
private:
	int connectionfd;

	std::map<std::string, std::string> ProtocolParser(const std::string headersBytes);
};
