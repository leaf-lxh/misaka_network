#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <string>
#include <iostream>
#include <fstream>

#include"modules/services.h"

#define LISTENQ 1024

void testFunction(int connectFD, sockaddr_in* client)
{
	std::string response = "Hello! ";
	char * clientAddr = new char[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &client->sin_addr.s_addr, clientAddr, INET_ADDRSTRLEN);

	response += clientAddr;
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
	delete recevied;
}



void ServiceDispatch(int connectFD)
{
	std::string response;
	std::string received;
	char *buffer = new char[4096];

	while (recv(connectFD,buffer,4096,0) > 0)
	{
		received += buffer;
	}

	std::string serviceName;
	size_t result = GetValueByKey(serviceName, "Service", received);
	if (result == std::string::npos)
	{
		response =
			"Status 404\r\n"
			"Reason:Service error\r\n\r\n";
	}
	else
	{
		if (serviceName == "Login")
		{
			LoginService thisSession;
			thisSession.Login(received,response);
		}
	}

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
	
	bind(socketFD, (sockaddr*)&serverAddr, sizeof(serverAddr));
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
				testFunction(connectFD,&clientAddr);
			}
			close(connectFD);
		}
	}
	close(socketFD);
	return 0;
	
}