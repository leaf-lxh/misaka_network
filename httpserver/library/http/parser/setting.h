#include <string>
#include <vector>
#include <map>
#include <stdexcept>

#include "HTTPParser.h"

namespace parser
{

	/*!
	strip函数，返回已去除两端特殊字符的源字符串
	参数: str | 源字符串
	参数: chr | 要被去除的特殊字符（串）
	返回: 已去除两端特殊字符的源字符串
	*/
	std::string strip(const std::string& str, const std::string chr = " ");

	class SettingReader
	{
	public:
		SettingReader() = default;
		using SettingDictionary = std::multimap<std::string, std::string>;
		using SettingXMLRootList = std::vector<XMLSetting>;

		/*!
		根据键=值的格式来从文件中解析配置
		参数：path | 配置文件的路径
		返回: 一个multimap对象，每个元素的first为键，second为值
		异常: 如果文件无法打开（文件不存在，权限不够等），抛出std::runtime_error(错误原因)
		*/
		SettingDictionary RetrieveFromKeyValueFmt(std::string path);

		/*!
		根据XML的格式来从文件中解析配置
		参数: path | 配置文件的路径
		返回: 一个元素类型为XMLSetting的vector
		异常: 如果文件无法打开（文件不存在，权限不够等），抛出std::runtime_error(错误原因)
		*/
		SettingXMLRootList RetrieveFromXMLFmt(std::string path);
	private:
		size_t GetCharacterLineNumber(size_t pos);
	};
}