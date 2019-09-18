#include <string>
#include <vector>

namespace webstring
{
	std::string strip(const std::string & str, const std::string chr = " ");
	std::string LeftStrip(const std::string& str, const std::string chr = " ");
	std::string RightStrip(const std::string& str, const std::string chr = " ");
	std::string tolower(const std::string& str);
	std::string toupper(const std::string& str);

	std::vector<char> URLdecode(std::string text);
	std::string URLencode(std::vector<unsigned char> text);
}