#include "network.h"
#include <stdexcept>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <openssl/ssl.h>

Network::Network(std::map<std::string, std::string> config)
{
	setting["SSL"] = "true";
	setting["address"] = "127.0.0.1";
	setting["port"] = "7200"

	if (config.find("SSL") != config.end())
	{
		setting["SSL"] = config["SSL"];
	}

	if (config.find("address") != config.end())
	{
		setting["address"] = config["address"];
	}

	if (config.find("port") != config.end())
	{
		setting["port"] = config["port"];
	}
}

bool Network::StartListen()
{
	return false;
}

std::map<std::string, std::string> Network::ProtocolParser(std::string packet)
{
	std::map<std::string, std::string> context;
	
}

std::string Network::EventHandler(std::string content)
{

}
