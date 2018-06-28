#include "./HTTPHandler.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int HTTPHandler::StartListen(unsigned short port)
{
	int socket_fd = 0;
	int accept_fd = 0;


	socket_fd = socket(AF_INET,SOCK_STREAM,0);

	struct sockaddr_in server_addr = {};
	struct sockaddr_in client_addr = {};

	server_addr.sin_port = htons(port);
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;


	bind(socket_fd,(sockaddr*)&server_addr,sizeof(sockaddr_in));

	return
}