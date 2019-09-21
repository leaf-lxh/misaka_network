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
	/*!
	用于服务器的配置文件读取
	参数：confPath | 配置文件的路径
	异常：读取异常会抛出runtime_error，what()中包含错误信息
	*/
	void Init(std::string confPath = "setting.conf") noexcept(false);

	/*!
	析构函数，用于服务器关闭时所有套接字描述符的关闭
	*/
	~TinyHttpd();

	/*!
	读取mime类型映射表（类型映射表抄自nginx =_=)
	参数：path | 映射表文件路径
	*/
	void LoadMIME(std::string path = "mime.types") noexcept;

	/*!
	添加服务器接受的微服务的路由路径
	参数：path | 路由路径，应以/开头，且不能重复
	返回：成功返回true，添加失败返回false
	*/
	bool AddRoutePath(std::string path) noexcept;

	/*!
	创建并监听服务器的监听套接字
	异常：从创建到监听的过程如果发生错误（如绑定失败，监听失败，地址端口为无效形式等），抛出runtime_error，what()中包含错误信息
	*/
	void StartListen() noexcept(false);

	/*!
	开始接收并处理客户端的请求
	*/
	void StartHandleRequest() noexcept;

	/*!
	打印当前服务器的serverProperty结构
	*/
	void DumpProperty() noexcept;
private:
	
	///HTTP请求报文结构
	struct HTTPRequestPacket
	{
		std::string method;
		std::string requestPath;
		std::string requestParam;
		std::string version;
		std::multimap<std::string, std::string> requestHeaders;
		std::string body;
	};

	///HTTP响应报文结构
	struct HTTPResponsePacket
	{
		std::string version;
		std::string code;
		std::multimap<std::string, std::string> responseHeaders;
		std::string body;
	};

	//调试信息输出等级，使用>=操作符来判断是否应输出。最啰嗦的是full模式，其次是essential模式，再是less模式。
	enum class VerboseLevel : unsigned char {
		full = 3,
		essential = 2,
		less = 1,
		silence = 0
	};
	///服务器配置
	struct ServerProperty
	{
		//调试模式的等级设定，0为不调试
		VerboseLevel verbose;
		//当前服务器的监听套接字
		int listenfd;
		//本服务器最多接受的客户数，即listen函数的第二个参数
		int maxClients;
		//绑定的地址
		std::pair<unsigned short, std::string> bind;
		//绑定的Unix套接字地址
		std::string unixBind;
		//微服务的路由表
		std::set<std::string> routeTable;
		//网站文件的根路径
		std::string documentRoot;
		//文件后缀-MIME类型映射表，first为文件类型，second为对应的mime类型
		std::map<std::string, std::string> mimeMap;
		//KeepAlive的超时时间
		int timeout;
		//每个KeepAlive连接能发送的最多请求数量
		int maxRequestsNum;
	}serverProperty;

	///每个已连接的客户的属性
	struct ClientProperty
	{
		ClientProperty() = default;
		//当前客户的fd
		int fd = -1;
		//读缓冲区的锁
		int readBufferLock;
		//写缓冲区的锁
		int writeBufferLock;
		//客户端发送过来的数据将存储至此
		std::string readBuffer;
		//要发送到客户端的数据存储至此
		std::string writeBuffer;
		//某次读取文件的请求，还有多少没有向缓冲区写的字节数。在该变量大于0的时候不应处理下一个请求
		std::streamsize aviliableFileBytesNum = 0;
		//当前正在处理的文件
		std::shared_ptr<std::fstream> file;
		//当前连接应在写缓冲区的数据发送完毕后当关闭。其读缓冲区的数据不应再读取
		bool readShutdown = false;
		//当前连接应当立即关闭，无论数据是否读取或发送完毕
		bool fullShutdown = false;
		//accept该客户端连接时使用的sockaddr_in结构
		struct sockaddr_in clientInfo;
		bool keepAlive;
		//上次活动的unix时间戳
		time_t lastAlive;
		//用于map比较用的判断操作符
		bool operator==(const ClientProperty& p)
		{
			return (this->fd == p.fd);
		}

	};

	//已连接的客户端信息
	std::map<int, struct ClientProperty> connectedClients;

	/*!
	从指定的客户端属性的读缓冲区(connectedClients[fd].second.readBuffer)中解析HTTP报文，如果构成一个完整的报文则调用HTTPPacketHandler，并在读缓冲区中清除掉当前报文
	参数：fd | 客户的套接字描述符
	*/
	void HTTPProfiler(int fd) noexcept;

	/*!
	向给定客户端发送writeBuffer中的数据
	参数：fd | 客户的套接字描述符
	*/
	void PushData(int fd) noexcept;

	/*!
	将客户端需要的文件数据放入客户端的writeBuffer中
	参数：fd | 客户的套接字描述符
	异常：如果stream不可用，抛出runtime_error异常
	*/
	void ReadFileStreamToBuffer(int fd) noexcept(false);

	/*!
	关闭某个客户端的连接，并进行清除操作（从connectedClinets容器中移除）
	参数：fd | 客户的套接字描述符
	*/
	void CloseConnection(int fd);

	/*!
	添加epoll事件
	参数：epollfd | epoll文件描述符
	参数：fd      | 要操作的描述符
	参数：flags   | 要添加的事件
	*/
	void AddEvent(int epollfd, int fd, int flags) noexcept;

	/*!
	修改epoll事件
	参数：epollfd | epoll文件描述符
	参数：fd      | 要操作的描述符
	参数：flags   | 要修改的事件
	*/
	void ModifyEvent(int epollfd, int fd, int flags) noexcept;

	/*!
	删除epoll事件
	参数：epollfd | epoll文件描述符
	参数：fd      | 要操作的描述符
	参数：flags   | 要删除的事件
	*/
	void DeleteEvent(int epollfd, int fd, int flags) noexcept;


	/*!
	HTTP错误请求的处理。如果在处理HTTP请求时发生错误（比如文件不存在，错误的请求报文格式等），则应调用本函数来进行该tcp连接的处理。
	最终的效果为向客户端发送一个错误信息页面，并将writeShutdown置为true
	参数：fd | 客户端连接描述符
	参数：code | HTTP错误码，如400 403 404
	参数：additional | 要显示给用户的附加信息
	*/
	void RaiseHTPPError(int fd, int code, std::string additional = "") noexcept;

	/*!
	将HTTPResponsePacket结构转化为字符串
	参数：response | HTTPResponsePacket结构
	返回：转换后的字符串
	*/
	std::string ResponseToString(HTTPResponsePacket response) noexcept;

	/*!
	堵塞式获取整个文件内容。如果文件很大，则不应使用本函数读取，因为会堵塞正常的请求
	参数：path | 文件路径
	返回：整个文件的内容
	异常：读取失败抛出runtime_error，what()返回对应的HTTP错误码：403为无权限读取，404为文件不存在，500为未知错误。
	*/
	std::string GetFileContent(std::string &path) noexcept(false);

	/*!
	获取给定文件的MIME类型
	参数：path | 文件路径
	返回：文件后缀名对应的MIME类型，无法识别则返回application/octet-stream
	*/
	std::string GetResponseType(std::string path) noexcept;

	//错误类型
	enum class FileAccessableStat : int {
		normalFile = 0,
		forbiden = 1,
		noneExist = 2,
		directory = 3,
		unknow = -1
	};
	/*!
	判断是否是一个可访问的文件
	参数：path | 文件路径
	返回：0-是正常的可读文件， 1-权限不足，2-路径不存在，3-路径指向一个目录，-1-未知错误
	*/
	FileAccessableStat IsAccessableFile(std::string path) noexcept;

	/*!
	获取文件的大小，单位为KBytes
	参数：path | 文件路径
	返回：文件的大小
	异常：文件读取失败则抛出runtime_error，what()返回错误原因
	*/
	std::streamsize GetFileLength(std::string path) noexcept(false);

	/*!
	对于每个HTTP请求的处理函数，默认函数为一个静态文件提供服务
	*/
	virtual void HTTPPacketHandler(int clientfd, HTTPRequestPacket request) noexcept;



};
