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
	//用于服务器的初始化
	void Init(std::string confPath = "setting.conf") noexcept(false);
	~TinyHttpd();

	//读取mime类型映射表（类型映射表抄自nginx =_=)
	void LoadMime(std::string path = "mime.types");
	//添加微服务的路由
	bool AddRoutePath(std::string path) noexcept;

	//开始服务器的请求监听
	void StartListen() noexcept(false);

	//开始接收并处理客户端的请求
	void StartHandleRequest() noexcept;

	//打印当前服务器的配置
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
		bool verbose;
		int listenfd;
		int maxClients;
		std::pair<unsigned short, std::string> bind;
		std::string unixBind;
		std::set<std::string> routeTable;
		std::string documentRoot;
		//first为文件类型，second为对应的mime类型
		std::map<std::string, std::string> mimeMap;
	}serverProperty;

	struct ClientProperty
	{
		ClientProperty() = default;
		int fd;
		//读缓冲区的锁
		int readBufferLock;
		//写缓冲区的锁
		int writeBufferLock;
		std::string readBuffer;
		std::string writeBuffer;
		//某次读取文件的请求，还有多少没有向缓冲区写的字节数。在该变量大于0的时候不应处理下一个请求
		std::streamsize aviliableFileBytesNum = 0;
		//当前正在处理的文件
		std::shared_ptr<std::fstream> file;
		//当前连接应在写缓冲区的数据发送完毕后当关闭。其读缓冲区的数据需抛弃
		bool writeShutdown;
		//当前连接应当立即关闭，无论数据是否读取或发送完毕
		bool fullShutdown;
		struct sockaddr_in clientInfo;
		//上次活动的unix时间戳
		time_t lastAlive;
		bool operator==(const ClientProperty& p)
		{
			return (this->fd == p.fd);
		}

	};

	//已连接的客户端信息
	std::map<int, struct ClientProperty> connectedClients;

	//从指定客户端的读缓冲区中解析HTTP报文，如果构成一个完整的报文则调用HTTPPacketHandler，并在读缓冲区中清除掉当前报文
	void HTTPProfiler(int fd);

	//向给定客户端发送writeBuffer中的数据
	void PushData(int fd);

	//将客户端需要的文件数据放入客户端的writeBuffer中
	void ReadFile(int fd);

	//关闭某个客户端的连接，并进行清除操作
	void CloseConnection(int fd);

	// epoll监听的套接字管理
	void AddEvent(int epollfd, int fd, int flags) noexcept;
	void ModifyEvent(int epollfd, int fd, int flags) noexcept;
	void DeleteEvent(int epollfd, int fd, int flags) noexcept;


	//HTTP错误请求处理
	void RaiseHTPPError(int fd, int code, std::string additional = "");


	std::string ResponseToString(HTTPResponsePacket response);

	std::string GetFileContent(std::string path);
	std::string GetResponseType(std::string path);
	virtual void HTTPPacketHandler(int clientfd, HTTPRequestPacket request);

};