#include <vector>
#include <string>
#include <map>

#pragma once

namespace HTTPPacket
{
	enum class ResponseCode : unsigned int {
		OK = 200,
		Created = 201,
		Accepted = 202,
		MovedPermanently = 301,
		Found = 302,
		NotModified = 304,
		BadRequest = 400,
		Unauthorized = 401,
		Forbidden = 403,
		NotFouond = 404,
		MethodNotAllowed = 405,
		InternalServerError = 500,
		NotImplemented = 501,
		BadGateway = 502,
		ServiceUnavailable = 503
	};
	/*!
	HTTP��Ӧ���ķ�װ��ʹ��װHTTP��Ӧ���ı������
	*/
	class HTTPResponsePacket
	{
	public:
		//HTTPЭ��汾
		std::string version = "HTTP/1.1";
		//HTTP��Ӧ״̬�룬����200
		ResponseCode code = ResponseCode::OK;
		//��Ӧ״̬���Ӧ����Ϣ������OK
		std::string msg = "OK";
		//��Ӧ����ͷ
		std::multimap<std::string, std::string> responseHeaders;
		//��Ӧ������
		std::string body;

		/*!
		�趨����״̬��ͷ�����Ϣ
		������code | ״̬��
		*/
		void SetResponseCode(ResponseCode rcode);

		/*!
		���Set-Cookie����ͷ������Ӷ��
		������cookie��ֵ
		*/
		void SetCookie(std::string cookie);

		/*!
		����Set-Cookie��ֻ����û��Set-Cookie����ͷ������²Ż����Set-Cookie����ͷ�����ڵ�����»�ֱ�����ҵ��ĵ�һ��Set-Cookie����ͷ���������
		ʹ�õȺţ�=��������ֵ�ķָʹ�÷ֺţ�;������ֵ��֮��ķָ�
		������key | ����ӵļ�
		������value | ����ӵ�ֵ
		*/
		void SetCookie(std::string key, std::string value);

		/*!
		�趨Content-Type
		*/
		void SetContentType(std::string type);

		/*!
		�趨Location
		*/
		void SetLocation(std::string location);

		/*!
		�趨Connection: Keep-Alive��������Keep-AliveΪָ����ֵ
		������timeout | ��ʱʱ��
		������maxRequests | ������������������
		*/
		void SetKeepAlive(unsigned int timeout, unsigned int maxRequests);

		/*!
		�趨Server
		*/
		void SetServer(std::string server);

		/*!
		�趨Content-Length��������Ĭ������������ֹ����ã���ToString�����л��Զ�����
		*/
		void SetContentLength(std::size_t  length);

		/*!
		�趨Content-Encoding��
		*/
		void SetContentEncoding(std::string encoding);

		/*!
		������Ӧ���Ķ���ת��Ϊ�ַ��������Զ�����body�ĳ������Content-Length����ͷ
		*/
		std::string ToString();

	};


	/*!
	HTTP�����ķ�װ��ʹ����HTTP�����ı������
	*/
	class HTTPRequestPacket
	{
	public:
		
		//���󷽷�
		std::string method;
		//���������url
		std::string fullURL;
		//����·��
		std::string requestPath;
		//����URL�еĲ���
		std::string requestParam;
		//HTTPЭ��汾
		std::string httpVersion;
		//����ͷ
		std::multimap<std::string, std::string> requestHeaders;
		//������Ϣ��
		std::string body;

		/*!
		����requestrParam(?key=value&key=value��ʽ)��ת��Ϊ��ֵ�Թ�������
		���أ���������������
		*/
		std::map<std::string, std::string> ParseURLParamter();

		/*!
		��鱾��������ָ������ͷ�Ƿ����
		������header     | ����ͷ������
		������ignoreCase | �Ƿ���Ա���ͷ�����Ĵ�Сд
		���أ�����ͷ��������δ�ҵ�Ϊ0
		*/
		unsigned int IsHeaderExist(const std::string& header, bool ignoreCase);

		/*!
		��ȡ��������ָ������ͷ������
		������header | ����ͷ������
		������ignoreCase | �Ƿ���Ա���ͷ�����Ĵ�Сд
		���أ����㱨��ͷ���Ƶ������б�
		*/
		std::vector<std::string> GetHeaderContent(const std::string& header, bool ignoreCase);

		/*!
		��ȡCookie��ָ���������
		������key | ������
		���أ���Ӧ������, ���Ϊ���򷵻ؿ��ַ���
		*/
		std::string GetCookieValue(const std::string& key);

		/*!
		��ȡ����ͷ��X-Real-IP������
		������ignoreCase | �Ƿ��������ͷ���ƵĴ�Сд
		*/
		std::string GetRealIP(bool ignoreCase = true);

		/*!
		��ȡ����ͷ��User-Agent������
		������ignoreCase | �Ƿ��������ͷ���ƵĴ�Сд
		*/
		std::string GetUserAgent(bool ignoreCase = true);

		/*!
		��ȡ����ͷ��Content-Type����
		������ignoreCase | �Ƿ��������ͷ���ƵĴ�Сд
		*/
		std::string GetContentType(bool ignoreCase = true);

		/*!
		��ȡ����ͷ��Content-Encoding������
		������ignoreCase | �Ƿ��������ͷ���ƵĴ�Сд
		*/
		std::string GetContentEncoding(bool ignoreCase = true);

		/*!
		��ȡ����ͷ�е�Content-Length�ֶ�
		������ignoreCase | �Ƿ��������ͷ���ƵĴ�Сд
		*/
		std::string GetContentLength(bool ignoreCase = true);

		/*!
		����������ת��Ϊ�ַ���
		*/
		std::string ToString();
	};
}