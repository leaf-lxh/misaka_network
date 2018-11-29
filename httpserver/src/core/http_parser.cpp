//#include "./HTTPHandler.h"
//
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
//#include <unistd.h>
//
//#include "../modules/FileIO.h"
//#include "../modules/encoding.h"
//#include "../modules/MySQL_connect.h"
//
//HTTPHandler::HTTPHandler()
//{
//	FileIO fileIO;
//	//Reading config.
//	std::string configContent;
//	fileIO.ReadFile(serverAttribute.configPath, configContent);
//	
//	ConfigGetLine(configContent, "LogPath", serverAttribute.logPath);
//	
//	fileIO.WriteLog(serverAttribute.logPath,"Initializing HTTPHandler.");
//}
//int HTTPHandler::StartListen(unsigned short port)
//{
//	
//	int socketFD = 0;
//	int acceptFD = 0;
//
//
//	socketFD = socket(AF_INET,SOCK_STREAM,0);
//
//	struct sockaddr_in serverAddr = {};
//	struct sockaddr_in clientAddr = {};
//
//	serverAddr.sin_port = htons(port);
//	serverAddr.sin_family = AF_INET;
//	serverAddr.sin_addr.s_addr = INADDR_ANY;
//
//
//	if (bind(socketFD, (sockaddr*)&serverAddr, sizeof(sockaddr_in)))
//	{
//		
//	}
//
//	
//	return 0;
//}
//
//size_t HTTPHandler::HandleRequest(const std::string &receive, std::string &response)
//{
//
//}
//
//bool HTTPHandler::ConfigGetLine(const std::string &config, const std::string & attributeName, std::string & buffer)
//{
//	size_t start = 0;
//	size_t end = 0;
//
//	start = config.find(attributeName);
//	if (start == config.npos)
//	{
//		return false;
//	}
//
//	start = attributeName.length() + 1;
//	end = config.find(start, '\n') + 1;
//	if (end == config.npos)
//	{
//		end = config.length();
//	}
//
//	buffer = config.substr(start, end);
//	return true;
//}
//
