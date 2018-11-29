//#include <string>
//
//class HTTPHandler{
//public :
//	HTTPHandler();
//	struct ServerAttribute
//	{
//		std::string configPath = "/etc/lxh.conf";
//		std::string logPath = "/var/log/lxh.log";
//		unsigned short listenPort = 80;
//	}serverAttribute;
//
//	int StartListen(unsigned short port);
//	size_t HandleRequest(const std::string &receive,std::string &response);
//
//private :
//	std::string requestHeaders;
//	std::string requestContent;
//	std::string responseHeaders;
//	std::string responseContent;
//
//	bool ConfigGetLine(const std::string &config,const std::string &attributeName, std::string &buffer);
//};
