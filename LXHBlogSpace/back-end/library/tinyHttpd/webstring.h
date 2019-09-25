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
	std::string URLencode(std::vector<unsigned char> text);

	/*!
	����HTTP�������(&key=value&key=value�Ĳ�����ʽ)
	������param | Ҫ�������ַ���
	������assginChar | ����ֵ֮��Ĺ�ϵ���ţ�Ĭ��Ϊ�Ⱥ�
	������splitChar  | ��ֵ��֮��ָ�ķ���
	���أ�һ�����������������ż�ֵ�ԵĲ���
	*/
	std::map<std::string, std::string> ParseKeyValue(std::string param, char assginChar = '=', char splitChar = '&');

	/*!
	ʹ��MD5�㷨����ָ���ַ���������ժҪ
	������text | Ҫ������ַ���
	���أ�����Ϊ16�ֽ�(128λ)������ժҪ��ʮ�����Ʊ�ʾ��ÿ���ֽڳ���Ϊ��λ������"helloworld"���"fc5e038d38a57032085441e7fe7010b0"
	*/
	std::string md5(std::string text);

	/*!
	ʹ��SHA1�㷨����ָ���ַ���������ժҪ
	������text | Ҫ������ַ���
	���أ�����Ϊ20�ֽ�(160λ)������ժҪ��ʮ�����Ʊ�ʾ��ÿ���ֽڳ���Ϊ��λ������"helloworld"���"6adfb183a4a2c94a2f92dab5ade762a47889a5a1"
	*/
	std::string sha1(std::string text);

	/*!
	��ȡһ��UUID
	���أ�һ���ַ������͵�UUID������f05c8857-d456-494d-a988-3904899a7aea
	*/
	std::string GenUUID();

	/*!
	���ɵ�ǰ��Unixʱ���
	*/
	std::string GenTimeStamp();
}