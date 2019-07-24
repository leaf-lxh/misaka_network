#include <string>
#include <map>
#include <stdexcept>

class Network
{
public:
	Network(std::map<std::string, std::string> config);

	/*!
	绑定地址和端口，并监听连接。绑定的描述符将传递到this->serverFD
	throw: runtime_error
	参数: 无
	返回: 完成绑定的套接字描述符
	*/
	int StartListen();

	void StartHandleRequest();
	
private:
	std::map<std::string, std::string> setting;
	int serverFD;

	/*!
	tinyLogger的协议处理函数，将数据包转换为键值映射
	*/
	virtual std::map<std::string, std::string> ProtocolParser(std::string packet);
	
	virtual std::string EventHandler(std::string content);
};
