#include <string>
#include <map>
#include <set>
#include <memory>
#include <fstream>
#include "netinet/in.h"
#include <ctime>

class TinyHttpd
{
public:
	//���ڷ������ĳ�ʼ��
	void Init(std::string confPath = "setting.conf") noexcept(false);

	//���΢�����·��
	bool AddRoutePath(std::string path) noexcept;

	//��ʼ���������������
	void StartListen() noexcept(false);

	//��ʼ���ղ�����ͻ��˵�����
	void StartHandleRequest() noexcept;

	//��ӡ��ǰ������������
	void DumpProperty() noexcept;
private:
	
	struct HTTPRequestPacket
	{
		std::string method;
		std::string requestPath;
		std::string version;
		std::multimap<std::string, std::string> requestHeaders;
		std::string body;
	};

	struct HTTPResponsePacket
	{
		std::string version;
		std::string code;
		std::multimap<std::string, std::string> responseHeaders;
		std::string body;
	};

	struct ServerProperty
	{
		int listenfd;
		int maxClients;
		std::pair<unsigned short, std::string> bind;
		std::string unixBind;
		std::set<std::string> routeTable;
	}serverProperty;

	struct ClientProperty
	{
		ClientProperty() = default;
		int fd;
		//������������
		int readBufferLock;
		//д����������
		int writeBufferLock;
		std::string readBuffer;
		std::string writeBuffer;
		//ĳ�ζ�ȡ�ļ������󣬻��ж���û���򻺳���д���ֽ������ڸñ�������0��ʱ��Ӧ������һ������
		std::streamsize aviliableFileBytesNum;
		//��ǰ���ڴ�����ļ�
		std::shared_ptr<std::fstream> file;
		//��ǰ����Ӧ��д�����������ݷ�����Ϻ󵱹رա����������������������
		bool writeShutdown;
		//��ǰ����Ӧ�������رգ����������Ƿ��ȡ�������
		bool fullShutdown;
		struct sockaddr_in clientInfo;
		//�ϴλ��unixʱ���
		time_t lastAlive;
		bool operator==(const ClientProperty& p)
		{
			return (this->fd == p.fd);
		}

	};

	//�����ӵĿͻ�����Ϣ
	std::map<int, struct ClientProperty> connectedClients;

	//��ָ���ͻ��˵Ķ��������н���HTTP���ģ��������һ�������ı��������HTTPPacketHandler�����ڶ����������������ǰ����
	void HTTPProfiler(int fd);

	//������ͻ��˷���writeBuffer�е�����
	void PushData(int fd);

	//���ͻ�����Ҫ���ļ����ݷ���ͻ��˵�writeBuffer��
	void ReadFile(int fd);

	//�ر�ĳ���ͻ��˵����ӣ��������������
	void CloseConnection(int fd);

	// epoll�������׽��ֹ���
	void AddEvent(int epollfd, int fd, int flags) noexcept;
	void ModifyEvent(int epollfd, int fd, int flags) noexcept;
	void DeleteEvent(int epollfd, int fd, int flags) noexcept;


	//HTTP����������
	void Raise400(int fd);
	void Raise404(int fd);

	virtual std::string HTTPPacketHandler(HTTPRequestPacket request);
};