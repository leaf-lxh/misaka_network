#include <string>

class HTTPHandler{
public :

	int StartListen(unsigned short port);
	size_t HandleRequest(const std::string &receive,std::string &response);

private :
	std::string request_headers;
	std::string request_content;
	std::string response_headers;
	std::string response_content; 
};
