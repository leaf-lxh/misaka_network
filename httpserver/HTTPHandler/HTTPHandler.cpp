#include <string>
#include <fstream>


size_t HTTPHandler::HandleRequest(const std::string &receive,std::string &response)
{
	size_t response_length = 0;
	
	return response_length;
}
//根据\r\n\r\n将收到的数据中的Header与Content分离开，分别存储到当前对象中的request_headers和request_headers变量中去
//找不到\r\n\r\n时，返回false，反之为true
bool HTTPHandler::FormatRequest (const std::string &receive)
{
	
	return count_header;
}

size_t HTTPHandler::CreateResponseContent(const char *file_name, std::string &content_buffer)
{
	size_t content_length = 0;
	
	return content_length;
}

bool ReadFile(const char *path,std::string &buffer)
{
	std::fstream file;
	return true;
}

bool WriteFile(const char *path,std::string &buffer)
{
	return true;
}
