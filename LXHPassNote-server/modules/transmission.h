class Transmission
{
public:
	struct Configuration
	{
		const char *bindAddress;
		short bindPort;

		const char *mysqlHost;
		const char *mysqlUser;
		const char *mysqlPassword;

		const char *emailAddress;
		const char *emailPassword;
		const char *emailSMTPServer;
		const char *emailSMTPPort;
		const char *emailSMTPSSLPort;
		
	}globalInfo;

	Transmission(Transmission::Configuration config) : globalInfo(config) {}

	bool ServerStart(std::string listenAddress, short port);
};

class LogFacility
{
public:
	struct Configuration
	{
		const char *logPath;
		const char *bindAddress;
		const char *bindPort;
	}globalInfo;

	LogFacility(LogFacility::Configuration config) : globalInfo(config) {}

	bool LogFacilityStart();

private:

};

