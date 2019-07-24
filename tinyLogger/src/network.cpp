#include "network.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <openssl/ssl.h>

Network::Network(std::map<std::string, std::string> config)
{
	this->setting["SSL"] = "on";
	this->setting["address"] = "127.0.0.1";
	this->setting["port"] = "7200";

	if (config.find("SSL") != config.end())
	{
		this->setting["SSL"] = config["SSL"];
	}

	if (config.find("address") != config.end())
	{
		this->setting["address"] = config["address"];
	}

	if (config.find("port") != config.end())
	{
		this->setting["port"] = config["port"];
	}
}

int Network::StartListen()
{
	serverFD = socket(AF_INET, SOCK_STREAM, 0);
	if (serverFD == -1)
	{
		throw std::runtime_error("Network::StartListen(): socket error.");
	}
	
	in_addr_t address;
	in_port_t port;
	try {
		int p = std::stoi(setting["port"]);
		if (p <= 0 || p > 65535)
		{
			throw std::runtime_error("?");
		}
		port = htons(p);
		if (inet_pton(AF_INET, setting["address"].c_str(), &address) <= 0)
		{
			throw std::runtime_error("?");
		}
	}
	catch (...) {
		throw std::runtime_error("Network::StartListen(): wrong format of address:port.");
	}

	struct sockaddr_in serverInfo = {};
	serverInfo.sin_family = AF_INET;
	serverInfo.sin_port = port;
	serverInfo.sin_addr.s_addr = address;

	if (bind(serverFD, (sockaddr *)&serverInfo, sizeof(sockaddr_in)) == -1)
	{
		throw std::runtime_error("Network::StartListen(): Unable to bind address:port.");
	}

	if (listen(serverFD, 2048) == -1)
	{
		throw std::runtime_error("Network::StartListen(): listen error.");
	}
	return serverFD;
}

std::map<std::string, std::string> Network::ProtocolParser(std::string packet)
{
	std::map<std::string, std::string> context;
	return context;
}

std::string Network::EventHandler(std::string content)
{
	return "";
}
