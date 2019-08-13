#include "network.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>

//#include <openssl/ssl.h>

#include <memory>

#define NETWORK_MAX_BUFFER_LEN 20971520

Network::Network()
{
	this->setting["address"] = "127.0.0.1";
	this->setting["port"] = "7200";
}

Network::Network(std::map<std::string, std::string> config)
{
	this->setting["address"] = "127.0.0.1";
	this->setting["port"] = "7200";

	if (config.find("address") != config.end())
	{
		this->setting["address"] = config["address"];
	}

	if (config.find("port") != config.end())
	{
		this->setting["port"] = config["port"];
	}

}

Network::~Network()
{
	close(listenFD);
	for(auto connection : connectionPool)
	{
		close(connection.first);
	}
}
int Network::StartListen()
{
	listenFD = socket(AF_INET, SOCK_STREAM, 0);
	if (listenFD == -1)
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
		throw std::runtime_error("Network::StartListen(): Invaild address or port.");
	}

	struct sockaddr_in serverInfo = {};
	serverInfo.sin_family = AF_INET;
	serverInfo.sin_port = port;
	serverInfo.sin_addr.s_addr = address;

	if (bind(listenFD, (sockaddr *)&serverInfo, sizeof(sockaddr_in)) == -1)
	{
		throw std::runtime_error("Network::StartListen(): Unable to bind address:port.");
	}

	//The max number of connections are limited by the size of struct fd_set which is used by select().
	//The size value is defined as FD_SETSIZE in <sys/types.h>
	//As I dont want to call select multiple times, 
	//I just set the backlog value of listen() to the max value that once the select() can check.
	if (listen(listenFD, FD_SETSIZE) == -1)
	{
		throw std::runtime_error("Network::StartListen(): listen error.");
	}

	//To avoid potential attack decripted in 《Unix网络编程 卷一 套接字API 第三版》，chapter 16.6
	//Set listenFD to none-block mode
	int flags = fcntl(listenFD, F_GETFL, 0);
	flags |= O_NONBLOCK;
	fcntl(listenFD, F_SETFL, flags);

	return listenFD;
}

void Network::StartHandleRequest()
{
	while(listenState == ListenState::RUN)
	{
		fd_set fdSet;
		FD_ZERO(&fdSet);

		int fdAmount = connectionPool.size();
		if (fdAmount < FD_SETSIZE)
		{
			FD_SET(listenFD, &fdSet);
		}
		
		int maxFD = listenFD;
		for(auto connection : connectionPool)
		{
			FD_SET(connection.first, &fdSet);
			if (connection.first > maxFD)
			{
				maxFD = connection.first;
			}
		}

		struct timeval timeout = { 5, 0 };
		int readyNum = select(maxFD + 1, &fdSet, nullptr, nullptr, &timeout);
		if (readyNum <= 0)
		{
			continue;
		}

		if (FD_ISSET(listenFD, &fdSet))
		{
			struct sockaddr_in clientInfo = {};
			clientInfo.sin_family = AF_INET;
			socklen_t len = sizeof(struct sockaddr_in);

			int newConnectionFD = accept(listenFD, (sockaddr *)&clientInfo, &len);
			if (newConnectionFD != -1)
			{
				std::unique_ptr<char[]> IPv4Address(new char[INET_ADDRSTRLEN + 1]());
				inet_ntop(AF_INET, &clientInfo.sin_addr.s_addr, IPv4Address.get(), INET_ADDRSTRLEN);

				connectionPool[newConnectionFD] = { std::string(IPv4Address.get()), std::to_string(clientInfo.sin_port) };
			}
			FD_CLR(listenFD, &fdSet);
			--readyNum;	
		}

		for (auto connection = connectionPool.cbegin(); (connection != connectionPool.cend()) && (listenState != ListenState::FORCE_SHUTDOWN); ++connection)
		{
			const int connectionFD = connection->first;
			if (!FD_ISSET(connectionFD, &fdSet))
			{
				continue;
			}
				
			while (true)
			{
				if (connectionBuffer[connectionFD].length() + 4096 > NETWORK_MAX_BUFFER_LEN)
				{
					break;
				}

				std::unique_ptr<char[]> buffer(new char[4096]());
				int receivedLen = recv(connectionFD, buffer.get(), 4096, MSG_DONTWAIT);
				if (receivedLen > 0)
				{
					connectionBuffer[connectionFD].append(buffer.get(), receivedLen);
					continue;
				}
						
				if (receivedLen == 0)
				{
					close(connectionFD);
					connectionPool.erase(connectionFD);
					connectionBuffer.erase(connectionFD);
				}
						
				if (receivedLen == -1 && (errno == EWOULDBLOCK || errno == EAGAIN))
				{
					std::string response;
					try
					{
						response = EventHandler(connectionBuffer[connectionFD]);
					}
					catch (...)
					{
						/* error occured */
						close(connectionFD);
						connectionPool.erase(connectionFD);
						connectionBuffer.erase(connectionFD);
						break;
					}

					/* to see if there are aviliable bytes to response. */
					if (response != "")
					{
						send(connectionFD, response.data(), response.length(), 0);
					}
				}
				
				break;
			}
		}
	}
}

void Network::GracefullyShutdown()
{
	this->listenState = this->ListenState::GRACE_SHUTDOWN;	
}

void Network::ForceShutdown()
{
	this->listenState = this->ListenState::FORCE_SHUTDOWN;
}

std::string Network::EventHandler(std::string &buffer)
{
	std::string response = "人工智障回复: ";
	size_t pos = buffer.find("?");
	if (pos != -1)
	{
		buffer[pos] = '!';
	}
	response += buffer;
	buffer = "";
	return response;
}
