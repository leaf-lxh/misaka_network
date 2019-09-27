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

	std::string URLdecode(std::string text);
	std::string URLencode(std::string text);

	/*!
	统计指定的UTF8编码的字符串中有多少个字符
	参数：utf8String | UTF8编码的字符串
	返回：字符个数
	*/
	std::size_t UTF8Strlen(const std::string& utf8String);

	/*!
	解析HTTP请求参数(&key=value&key=value的参数格式)
	参数：param | 要解析的字符串
	参数：assginChar | 键与值之间的关系符号，默认为等号
	参数：splitChar  | 键值对之间分割的符号
	返回：一个关联容器，包含着键值对的参数
	*/
	std::map<std::string, std::string> ParseKeyValue(std::string param, char assginChar = '=', char splitChar = '&');

	/*!
	使用MD5算法计算指定字符串的数据摘要
	参数：text | 要计算的字符串
	返回：长度为16字节(128位)的数据摘要的十六进制表示，每个字节长度为两位。输入"helloworld"输出"fc5e038d38a57032085441e7fe7010b0"
	*/
	std::string md5(std::string text);

	/*!
	使用SHA1算法计算指定字符串的数据摘要
	参数：text | 要计算的字符串
	返回：长度为20字节(160位)的数据摘要的十六进制表示，每个字节长度为两位。输入"helloworld"输出"6adfb183a4a2c94a2f92dab5ade762a47889a5a1"
	*/
	std::string sha1(std::string text);

	/*!
	获取一个UUID
	返回：一个字符串类型的UUID，例如f05c8857-d456-494d-a988-3904899a7aea
	*/
	std::string GenUUID();

	/*!
	生成当前的Unix时间戳
	*/
	std::string GenTimeStamp();

	/*!
	将string类型的map转化为JSON字符串
	*/
	std::string JsonStringify(std::map<std::string, std::string> object);
}