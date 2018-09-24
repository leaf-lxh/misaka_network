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
	*��log�ļ��ײ�׷������, log�ļ�·��ȡ�������ļ�
	*������log        | ��׷����log�ļ�������
	*���أ�bool ���д��ɹ�����true����֮ false
	*************************************************************************************************************************/
	bool WriteLog(const std::string log);

	/************************************************************************************************************************
	*�������ļ���ȡ����
	*��������
	*���أ�bool ��ȡ�ɹ�����true����֮ false
	*************************************************************************************************************************/
	bool ReadConfig();
private:
	std::string _configFilePath;

	/************************************************************************************************************************
	*��ѯ·���Ƿ����
	*������path        | ����ѯ��·��
	*���أ�bool ������ڣ�����true ��֮ false
	*************************************************************************************************************************/
	bool PathCheck(const std::string  path);

	/************************************************************************************************************************
	*�Ը�Ŀ¼Ϊ��ǰĿ¼������Ŀ¼��֧�ִ������Ŀ¼
	*������path        | ��������·����·������ Ŀ¼/�ļ�������ֻ����Ŀ¼���������ļ�
	           mode      | Ŀ¼Ȩ��
	*���أ�bool �����ɹ�����true ��֮ false
	*************************************************************************************************************************/
	bool PathCreate(const std::string  path, unsigned int mode);

};

