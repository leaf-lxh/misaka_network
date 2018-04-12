#include <string>

class httppraser{
public :
	
	size_t HandleRequest(const std::string &receive,std::string &response);
	struct key_value
	{
		std::string key;
		std::string value;
	};
private :
	bool ReadFile(const char *path,std::string &buffer);
	bool WriteFile(const char *path,std::string &buffer);

	int FormatRequestHeaders (const std::string &receive);
	int CreateReponseHeaders(const char *status_code);
	size_t CreateResponseContent(const char *file_name, std::string &content_buffer);
};
