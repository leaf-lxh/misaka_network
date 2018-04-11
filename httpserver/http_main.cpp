#include "unp.h"
#include <fstream>
#include <sstream>
#include "./httppraser/praser.h"
#include "./script/script.h"

//A function that read specific file,and store hole content to a given string buffer.
//path: specific file path
//buffer:a string varable that store file data.As string's max_size(), this function can only read the file that is smaller than 4GB.
//if function is successfully called,return 1,otherwise return 0.
int  ReadFile(const char *path,std::string &buffer)
{
	using std::fstream;
	fstream file(path,fstream::in) //read only
	
	if(!file.good())
	{
		return 0;
	}

	std::string temp = "";
	buffer = "";
	
	while(getline(file,temp))
	{
		buffer+=temp+"\n";
	}

	return 1;
}
void HTTPResponseToClient(int connectfd, const std::string &text)
{
	size_t writed = 0;
	size_t write_size = 0;
	size_t total_length = text.length();
	while (true)
	{
		write_size = total_length - writed;
		if ( write_size >= MAXLINE)
		{
			write_size = MAXLINE;
		}
			
		writed += write(connectfd,text.substr(writed, writed + writed_size).c_str(), write_size);
		
		if (writed >= total_length)
		{
			break;
		}
	}	
	return;
}
void HTTPRequestHandler(int connectfd)
{
	char buffer[MAXLINE];
	ssize_t length = 0;
	size_t max_length = 10240000; //10MB 
	size_t received_total_length = 0;
	size_t reponse_length = 0;
	std::string receive;
	std::string response;

	httppraser praser;

	while (true)
	{
		length = read(connectfd, buffer,MAXLINE); //read data from cache area

		if (length > 0)
		{
			total_length += length;
			if (total_length > max_length )	
			{
				return; //If the data we received is bigger than max_length,then we drop this request.
			}
			receive += buffer;
		}

		if (length < 0 && errno == EINTR) //if read() failed and was caused by interrupted calling, retry read())
		{
			continue; //I dont know how to dealing with retry times...
		
		}

		if (length <= 0)
		{
			break;
		}
	}
	if (total_length == 0)
	{
		return; //nothing received,drop this request.
	}		
	response_length = praser.HandleRequest(receive,response);
	if (response_length)
	{
		HTTPResponseToClient(connectfd, response);
	}
	return;
}

int main (int argvc , char **argv)
{
	int server_port = 80;
	int listenfd = 0;
	int connectfd = 0;
	pid_t childpid = 0;

	struct sockaddr_in serveraddr = {};
	struct sockaddr_in clientaddr = {};

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(server_port);

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	bind(listenfd, (SA*)&serveraddr, sizeof(serveraddr));
	listen(listenfd, LISTENQ);
	
	while (true)
	{
		connectfd = accept(listenfd, &clientaddr, sizeof(clientaddr));
		childpid = fork();
		
		if (childpid == 0)
		{
			close(listenfd);
			HTTPRequestMainHandler();
		}
		close(connectfd);		
	}

	return 0;
}
