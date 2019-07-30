#include <string>
#include <map>
#include <utility>
#include <stdexcept>

class Network
{
public:
	Network(std::map<std::string, std::string> config);

	/*!
	绑定地址和端口，并监听连接。绑定的描述符将传递到this->serverFD
	异常：std::runtime_error(原因)
	参数：无
	返回：完成绑定的套接字描述符
	*/
	int StartListen();

	/*!
	开始接收客户端连接，处理客户请求。
	异常: 无
	参数：无
	返回：无
	*/
	void StartHandleRequest();

	/*!
	缓慢关闭监听，即处理完当前全部用户请求后再关闭监听服务
	异常：无
	参数: 无
	返回：无
	*/
	void GracefullyShutdown();

	/*!
	强制关闭监听，即立刻关闭当前的所有套接字描述符
	异常：无
	参数：无
	返回：无	
	*/
	void ForceShutdown();
	
private:
	/*可选用的请求监听状态*/
	enum class ListenState : unsigned char { RUN = 0, GRACE_SHUTDOWN = 1, FORCE_SHUTDOWN = 2};
	
	/*服务的配置，键-值类型*/
	std::map<std::string, std::string> setting;
	
	/*服务的监听套接字*/
	int listenFD;

	/*请求监听状态*/
	ListenState listenState = ListenState::RUN;

	/*已建立连接的描述符池，数据分别为：描述符，IPv4源地址-源端口*/
	std::map<int, std::pair<std::string, std::string> > connectionPool;
	
	/*连接的数据缓冲区，以描述符-缓冲区的形式存储*/
	std::map<int, std::string> connectionBuffer;

	/*!
	请求响应函数，根据需求进行重载
	异常：无
	参数：buffer | 当前连接的数据缓冲区
	返回：返回给客户端的数据
	*/
	virtual std::string EventHandler(std::string &buffer);
};
