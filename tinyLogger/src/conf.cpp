#include "conf.h"

#include <fstream>
#include <regex>
#include <climits>

unsigned int SettingReader::ReadConfiguration()
{
	std::fstream file(this->path, std::fstream::in);
	std::string buffer;
	unsigned int count = 0;

	std::regex format("([^#]+?)=(.+?)(#.*)?");
	std::smatch result;
	while(getline(file, buffer))
	{
		if(std::regex_match(buffer, result, format) == true)
		{
			this->config[strip(result[1])] = strip(result[2]);
			if(++count == UINT_MAX)
			{
				break;
			}
		}
	}

	return count;	
}

std::string SettingReader::strip(const std::string str)
{
	std::string victim(str);
	for(auto it=victim.cbegin(); it != victim.cend(); ++it)
	{
		if(*it != ' ')
		{
			victim = victim.substr(it-victim.cbegin());
			break;
		}
	}

	for(auto rit=victim.crbegin(); rit != victim.crend(); ++rit)
	{
		if(*rit != ' ')
		{
			victim = victim.substr(0, victim.crend() - rit);
			break;
		}
	}

	return victim;
}
