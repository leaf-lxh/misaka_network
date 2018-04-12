#include <string>
#include <fstream>


size_t httppraser::HandleRequest(const std::string &receive,std::string &response)
{
	size_t response_length = 0;
	int count_headers = FormatRequestHeaders(receive);
	return response_length;
}

int httppraser::FormatRequestHeaders (const std::string &receive)
{
	int count_header = 0;
	return count_header;
}

size_t httppraser::CreateResponseContent(const char *file_name, std::string &content_buffer)
{
	size_t content_length = 0;
	
	return content_length;
}

bool ReadFile(const char *path,std::string &buffer)
{
	return true;
}

bool WriteFile(const char *path,std::string &buffer)
{
	return true;
}
