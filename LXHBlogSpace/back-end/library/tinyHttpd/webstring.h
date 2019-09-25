#include <string>
#include <vector>
#include <map>

namespace webstring
{
	std::string strip(const std::string & str, const std::string chr = " ");
	std::string LeftStrip(const std::string& str, const std::string chr = " ");
	std::string RightStrip(const std::string& str, const std::string chr = " ");
	std::string tolower(const std::string& str);
	std::string toupper(const std::string& str);

	std::vector<char> URLdecode(std::string text);
	std::string URLencode(std::vector<unsigned char> text);

	/*!
	解析HTTP请求参数(&key=value&key=value的参数格式)
	参数：param | 要解析的字符串
	参数：assginChar | 键与值之间的关系符号，默认为等号
	参数：splitChar  | 键值对之间分割的符号
	返回：一个关联容器，包含着键值对的参数
	*/
	std::map<std::string, std::string> ParseKeyValue(std::string param, char assginChar = '=', char splitChar = '&');
}