#include "setting.h"

#include <fstream>
#include <regex>
#include <memory>

std::multimap<std::string, std::string> parser::SettingReader::RetriveFromKeyValueFmt(std::string path)
{
	std::fstream file(path, std::fstream::in);
	if (file.good() == false)
	{
		throw std::runtime_error(std::string("Failed to open file: ") + path);
	}
	file.seekg(0, file.end);
	size_t fileLength = file.tellg();
	file.seekg(0, file.beg);

	if (fileLength == 0)
	{
		return {};
	}
	std::unique_ptr<char[]> buffer(new char[fileLength]());
	file.read(buffer.get(), fileLength);
	file.close();

	std::string fileContent(buffer.get());
	std::regex configFormat("(.+?)=(.+?)\n");
	std::regex commentFormat("([^#]*?)#(.*?)");
	std::sregex_iterator rend;
	std::multimap<std::string, std::string> settingMap;

	for (auto it = std::sregex_iterator(fileContent.begin(), fileContent.end(), configFormat); it != rend; ++it)
	{
		std::smatch result;
		std::string buffer((*it)[2].str());
		if (std::regex_match(buffer, result, commentFormat))
		{
			settingMap.insert({ strip((*it)[1].str()), strip(result[1].str()) });
		}
		else
		{
			settingMap.insert({ strip((*it)[1].str()), strip((*it)[2].str()) });
		}
	}

	return settingMap;

}

std::vector<parser::XMLSetting> parser::SettingReader::RetriveFromXMLFmt(std::string path)
{
	return std::vector<parser::XMLSetting>();
}

std::string parser::SettingReader::strip(const std::string &str, const std::string chr)
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
