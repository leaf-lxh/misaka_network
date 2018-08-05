#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <string>
#include <iostream>
#include <fstream>

#define LISTENQ 1024

void testFunction(int connectFD, sockaddr_in* client, socklen_t len)
{
	std::string response = "Hello! ";
	char * buf = new char[256];
	inet_ntop(AF_INET, client, buf, len);

	response += buf;
	response += "\nYour content:\n";
	ssize_t recv_len = 0;

	char * recevied = new char[4096];
	while (recv_len = recv(connectFD, recevied, 4096, 0) > 0)
	{
		response += recevied;
	}

	std::fstream file;
	file.open("./out.log", std::fstream::app | std::fstream::out);
	file << response << std::endl;
	file.close();

	send(connectFD, response.c_str(), response.length(), 0);
}


int main()
{
	int socketFD = socket(AF_INET,SOCK_STREAM,0);

	sockaddr_in serverAddr = {};
	sockaddr_in clientAddr = {};
	socklen_t clientlen = sizeof(clientAddr);
	
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(30000);
	
	bind(socketFD, (sockaddr*)&clientAddr, sizeof(clientAddr));
	listen(socketFD, LISTENQ);

	int connectFD = 0;

	while (true)
	{
		connectFD = accept(socketFD, (sockaddr*)&clientAddr, &clientlen);
		std::cout << connectFD << std::endl;
		if (connectFD>0)
		{
			if (fork() == 0)
			{
				testFunction(connectFD,&clientAddr,clientlen);
			}
			close(connectFD);
		}
	}
	close(socketFD);
	return 0;
	
}