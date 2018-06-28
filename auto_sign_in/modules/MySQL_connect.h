#include <string>
#include <sstream>

class MySQLConnect
{
public:
	//进行mysql_connect类的初始化，并测试能否连接成功。
	//在使用本类中的任何方法前必须调用本函数
	bool init(std::string host, unsigned int port, std::string &username, std::string &password);

	//用于结束进程时释放mysql库占用的内存
	//应在进程结束时调用本函数
	void destory();

	
	bool query();
private:
	struct MySQLInfo
	{
		std::string host;
		unsigned int port;
		std::string username;
		std::string password;
	}_MySQLInfo;
};


