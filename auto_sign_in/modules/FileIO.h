#pragma once
#include <string>
#include <stdexcept>

struct ConfigStruct
{
	int hour;
	int minute;
	std::string log_path;
};
class SignInFileIO
{
public:
	inline SignInFileIO()
	{
		config.log_path = "/var/log/auto_sign_in/log.txt";
		_configFilePath = "/etc/auto_sign_in/config.conf";
		if (!ReadConfig())
		{
			WriteLog("Failed to read config file, destorying this instance");
			throw std::runtime_error("Failed to read config file, destorying this instance");
			this->~SignInFileIO();
		}
	
	}

	~SignInFileIO() = default;

	ConfigStruct config;
	
	/************************************************************************************************************************
	*从log文件底部追加内容, log文件路径取自配置文件
	*参数：log        | 欲追加至log文件的内容
	*返回：bool 如果写入成功返回true，反之 false
	*************************************************************************************************************************/
	bool WriteLog(const std::string log);

	/************************************************************************************************************************
	*从配置文件读取配置
	*参数：无
	*返回：bool 读取成功返回true，反之 false
	*************************************************************************************************************************/
	bool ReadConfig();
private:
	std::string _configFilePath;

	/************************************************************************************************************************
	*查询路径是否存在
	*参数：path        | 欲查询的路径
	*返回：bool 如果存在，返回true 反之 false
	*************************************************************************************************************************/
	bool PathCheck(const std::string  path);

	/************************************************************************************************************************
	*以根目录为当前目录，创建目录。支持创建多层目录
	*参数：path      | 欲创建的路径，路径按照 目录/文件名处理，只创建目录，不创建文件
	*参数：mode      | 目录权限
	*返回：bool 创建成功返回true 反之 false
	*************************************************************************************************************************/
	bool PathCreate(const std::string  path, unsigned int mode);

};

