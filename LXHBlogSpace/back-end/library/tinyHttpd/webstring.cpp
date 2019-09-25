#include "webstring.h"
#include <regex>
#include <sstream>
#include <iomanip>
#include <ctime>

//compile with option -lcrypto
#include <openssl/md5.h>
#include <openssl/sha.h>

#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.

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


	std::string URLdecode(std::string text)
	{
		//这vector用的..凑合着用吧，还能离不成
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

		return std::string(convertedBytes.data(), convertedBytes.size());
	}

	std::string URLencode(std::string text)
	{
		std::string convertedBytes;
		std::string temp;
		std::stringstream stream;
		for (auto chr : text)
		{

			if (!((chr >= 0x41 && chr <= 0x5A) || (chr >= 0x61 && chr <= 0x7A) || (chr >= 0x30 && chr <= 0x39)) && (chr != '-' && chr != '_' && chr != '.' && chr != '`'))
			{
				stream << std::setfill('0') << std::setw(2) << std::hex << (static_cast<unsigned int>(chr) & 0xFF);
				stream >> temp;
				convertedBytes += '%' + temp;
				stream.clear();
			}
			else
			{
				convertedBytes += chr;
			}
		}

		return convertedBytes;
	}

	std::map<std::string, std::string> ParseKeyValue(std::string param, char assginChar, char splitChar)
	{
		using namespace std;

		std::map<std::string, std::string> result;
		if (assginChar == splitChar)
		{
			return result;
		}

		std::string key;
		std::string value;
		std::string temp_str;

		size_t begin_pos = 0;
		size_t str_length = param.length();
		size_t split_pos;
		size_t equal_pos;

		while (begin_pos < str_length)
		{
			split_pos = param.find(splitChar, begin_pos);
			if (split_pos == string::npos)
			{
				//匹配到最后一个键值对
				equal_pos = param.find(assginChar, begin_pos);
				if (equal_pos != string::npos)
				{
					key = param.substr(begin_pos, equal_pos - begin_pos);
					value = param.substr(equal_pos + 1);
					result.insert({ key, value });
				}

				break;
			}
			else
			{
				temp_str = param.substr(begin_pos, split_pos);
				equal_pos = temp_str.find(assginChar);
				if (equal_pos != string::npos)
				{
					key = temp_str.substr(0, equal_pos);
					value = temp_str.substr(equal_pos + 1);
					result.insert({ key, value });
				}

				begin_pos = split_pos + 1;
				continue;
			}
		}
		
		return result;
	}

	std::string md5(std::string text)
	{
		std::unique_ptr<unsigned char> digest(new unsigned char[MD5_DIGEST_LENGTH]());
		MD5((unsigned char*)text.c_str(), text.length(), digest.get());

		std::stringstream hexdigest;
		hexdigest << std::hex;
		for (int i = 0; i < MD5_DIGEST_LENGTH; ++i)
		{
			hexdigest << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(digest.get()[i]);
		}

		return hexdigest.str();
	}

	std::string sha1(std::string text)
	{
		std::unique_ptr<unsigned char> digest(new unsigned char[SHA_DIGEST_LENGTH]());
		SHA1((unsigned char*)text.c_str(), text.length(), digest.get());

		std::stringstream hexdigest;
		hexdigest << std::hex;
		for (int i = 0; i < SHA_DIGEST_LENGTH; ++i)
		{
			hexdigest << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(digest.get()[i]);
		}

		return hexdigest.str();
	}

	std::string GenUUID()
	{
		boost::uuids::uuid uuid = boost::uuids::random_generator()();
		return boost::uuids::to_string(uuid);
	}

	std::string GenTimeStamp()
	{
		return std::to_string(time(nullptr));
	}
}