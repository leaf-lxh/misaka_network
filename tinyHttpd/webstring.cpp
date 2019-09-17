#include "webstring.h"

namespace webstring
{
	std::string strip(const std::string& str, const std::string chr = " ")
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

	std::string tolower(const std::string& str)
	{
		std::string result;
		for (auto c : str)
		{
			if ((c >= 'A') && (c <= 'Z'))
			{
				result += c + ('a' - 'A');
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
				result += c - ('a' - 'A');
			}
		}
		return result;
	}
}