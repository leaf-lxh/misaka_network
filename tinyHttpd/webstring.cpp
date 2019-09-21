#include "webstring.h"
#include <regex>
#include <sstream>
#include <iomanip>

namespace webstring
{
	std::string strip(const std::string& str, const std::string chr)
	{
		std::string result(str);
		bool found = false;
		while (true)
		{
			found = false;
			for (auto c = chr.cbegin(); (c != chr.cend()) && (found == false); ++c)
			{
				auto start = result.find_first_not_of(*c);

				if ((start != result.npos) && (start != 0))
				{
					/*如果从左开始能找到非目标字符，且位置不为0*/
					result = result.substr(start);
					found = true;
				}


				auto stop = result.find_last_not_of(*c);
				/*如果从右边开始能找到非目标字符，且位置不为最后*/
				if ((stop != result.npos) && (stop != result.length() - 1))
				{
					result = result.substr(0, stop + 1);
					found = true;
				}
			}

			if (found == false)
			{
				break;
			}
		}

		return result;
	}

	std::string LeftStrip(const std::string& str, const std::string chr)
	{
		std::string result(str);
		bool found = false;
		while (true)
		{
			found = false;
			for (auto c = chr.cbegin(); (c != chr.cend()) && (found == false); ++c)
			{
				auto start = result.find_first_not_of(*c);

				if ((start != result.npos) && (start != 0))
				{
					/*如果从左开始能找到非目标字符，且位置不为0*/
					result = result.substr(start);
					found = true;
				}
			}

			if (found == false)
			{
				break;
			}
		}

		return result;
	}

	std::string RightStrip(const std::string& str, const std::string chr)
	{
		std::string result(str);
		bool found = false;
		while (true)
		{
			found = false;
			for (auto c = chr.cbegin(); (c != chr.cend()) && (found == false); ++c)
			{
				auto stop = result.find_last_not_of(*c);
				/*如果从右边开始能找到非目标字符，且位置不为最后*/
				if ((stop != result.npos) && (stop != result.length() - 1))
				{
					result = result.substr(0, stop + 1);
					found = true;
				}
			}

			if (found == false)
			{
				break;
			}
		}

		return result;
	}

	std::string tolower(const std::string& str)
	{
		std::string result;
		for (auto c : str)
		{
			if ((c >= 'A') && (c <= 'Z'))
			{
				result += c + 32;
			}
			else
			{
				result += c;
			}
		}
		return result;
	}

	std::string toupper(const std::string& str)
	{
		std::string result;
		for (auto c : str)
		{
			if ((c >= 'a') && (c <= 'z'))
			{
				result += c - 32;
			}
			else
			{
				result += c;
			}
		}
		return result;
	}

	std::vector<char> URLdecode(std::string text)
	{
		std::vector<char> convertedBytes;
		std::stringstream stream;

		std::regex format("%([0-9a-fA-F]{2})");

		for (auto index = text.cbegin(); index != text.cend(); )
		{
			if (*index == '%')
			{
				std::string assumedEncodedString(index, index + 3);
				std::smatch result;
				if (std::regex_match(assumedEncodedString, result, format))
				{
					unsigned int byteValue;
					stream << std::hex << result[1];
					stream >> byteValue;
					stream.clear();

					convertedBytes.push_back(static_cast<char>(byteValue));
					index += 3;
				}
				else
				{
					convertedBytes.push_back(*index);
					index++;
				}
			}
			else
			{
				convertedBytes.push_back(*index);
				index++;
			}
		}

		return convertedBytes;
	}

	std::string URLencode(std::vector<unsigned char> text)
	{
		std::string convertedBytes;
		std::string temp;
		std::stringstream stream;
		for (auto index : text)
		{

			if (!((index >= 0x41 && index <= 0x5A) || (index >= 0x61 && index <= 0x7A) || (index >= 0x30 && index <= 0x39)) && (index != '-' && index != '_' && index != '.' && index != '`'))
			{
				stream << std::setfill('0') << std::setw(2) << std::hex << (static_cast<unsigned int>(index) & 0xFF);
				stream >> temp;
				convertedBytes += '%' + temp;
				stream.clear();
			}
			else
			{
				convertedBytes += index;
			}
		}

		return convertedBytes;
	}
}