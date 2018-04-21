#include <string>

class HTTPHandler{
public :
	
	size_t HandleRequest(const std::string &receive,std::string &response);
	std::string request_headers;
	std::string request_content;
	std::string response_headers;
	std::string response_content; 
private :
	bool ReadFile(const std::string path,std::string &buffer);
	bool WriteFile(const std::string path,std::string &buffer);

	int FormatRequestHeaders (const std::string &receive);
	int CreateReponseHeaders(const char *status_code);
	size_t CreateResponseContent(const char *file_name, std::string &content_buffer);
	size_t GetRequestPath(std::string &request_line)
};
