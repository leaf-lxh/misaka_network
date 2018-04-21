#include "unp.h"
#include <fstream>
#include <sstream>
#include "./HTTPHandler/HTTPHandler.h"
#include "./script/script.h"

//A function that read specific file,and store hole content to a given string buffer.
//path: specific file path
//buffer:a string varable that store file data.As string's max_size(), this function can only read the file that is smaller than 4GB.
//if function is successfully called,return 1,otherwise return 0.
int  ReadFile(const char *path,std::string &buffer)
{
	using std::fstream;
	fstream file(path, fstream::in); //read only
	
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
			
		writed += write(connectfd,text.substr(writed, writed + write_size).c_str(), write_size);
		
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
	size_t response_length = 0;
	std::string receive;
	std::string response;

	int retry_times = 5;

	HTTPHandler http_handler;

	while (true)
	{
		//从缓冲区读取数据
		length = read(connectfd, buffer,MAXLINE); 

		if (length > 0)
		{
			received_total_length += length;

			//如果读取的数据大小超过了max_length,那么停止接收数据，并抛掉这个请求，因为长度过长
			if (received_total_length > max_length )
			{
				return; 
			}
			receive += buffer;
		}

		//如果read失败，并且errno == EINTR(因系统中断导致)，则尝试重新read
		if (length < 0 && errno == EINTR) 
		{
			if (retry_times == 0)
			{
				//如果尝试次数==0，则抛掉这个请求
				return;
			}

			//剩余尝试次数减一并重试read()
			--retry_times;
			continue; 
		
		}

		//如果读取长度小于等于0则意味着没有剩余数据可供读取
		if (length <= 0)
		{
			break;
		}
	}

	//啥也没收到，抛掉这个请求
	if (received_total_length == 0)
	{
		return; 
	}		
	response_length = http_handler.HandleRequest(receive,response);
	if (response_length)
	{
		HTTPResponseToClient(connectfd, response);
	}
	return;
}

int main (int argvc , char **argv)
{
	int server_port = 80;
	int listen_fd = 0;
	int connect_fd = 0;
	pid_t child_pid = 0;

	struct sockaddr_in serveraddr = {};
	struct sockaddr_in clientaddr = {};
	socklen_t s_length = sizeof(sockaddr_in);



	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(server_port);

	listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	bind(listen_fd, (SA*)&serveraddr, sizeof(serveraddr));
	listen(listen_fd, LISTENQ);
	
	while (true)
	{
		connect_fd = accept(listen_fd, (SA*)&clientaddr, &s_length);
		child_pid = fork();
		
		if (child_pid == 0)
		{
			close(listen_fd);
			HTTPRequestHandler(connect_fd);
		}
		close(connect_fd);		
	}

	return 0;
}
