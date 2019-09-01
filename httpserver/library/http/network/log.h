#include <string>

class LogService
{
public:
	virtual void WriteLog(std::string text);

	LogService& operator<<(std::string text);
};

class tinyLoggerService : public LogService
{
public:
	tinyLoggerService() = default;
	void WriteLog(std::string text);
private:
	void EmergencyWrite(std::string text);
};