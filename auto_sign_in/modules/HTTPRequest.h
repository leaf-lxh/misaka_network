#include <string>
#include <vector>


#pragma once

class HTTPRequest {
public:
	HTTPRequest() = default;
	~HTTPRequest() = default;

	std::string lastErrorString;
	
	/************************************************************************************************************************
	*����HTTP���󡣲�֧��HTTPS
	*������requestURL              |�����URL��Ӧ�ϸ��ո�ʽhttp://host/directory
	*      additionalHeaders       |���ӵ�Headers //ӦΪadditionalData���Content-type��Content-encoding,ÿ��headerӦ�Ӻ�׺\r\n
	*      method                  |����ʽ
	*      additionalData          |��Headers���͵ĸ��ӵ�����
	*      dataLength              |additionalData�ĳ���
	*      port                    |HTTP�������Ķ˿�
	*���أ�bool                    |����ɹ�����true,����Ϊfalse,�������鿴lastErrorString
	*************************************************************************************************************************/
	bool HTTPOpenRequest(std::string requestURL, std::vector<std::string> additionalHeaders, std::string method, std::vector<char> additionalData, unsigned short port);

	std::vector<char> GetResponseHeaders();

	std::vector<char> GetResponseMessageBody();

	std::string GetHeaderFieldValue(std::string fieldName);

	std::vector<char> URLencode(std::vector<char>);

	std::vector<char> URLdecode(std::vector<char>);

	std::vector<char> UnicodeEscapeToUTF8(std::string text);

	std::string UTF8ToUnicodeEscape(std::vector<char>);
	
private:
	std::vector<char> _response;

	/************************************************************************************************************************
	*��ָ����host:port��������
	*������ host|Ŀ���������������IP��ַ
	*       port|Ŀ�����˿�
	*       data|�����͵�����
	*���أ� bool �޴��󷵻�true�� ʧ�ܷ���false���������鿴lastErrorString
	*************************************************************************************************************************/
	bool request(std::string host,unsigned short port,std::string &data);
};