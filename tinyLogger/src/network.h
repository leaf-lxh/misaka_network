#include <string>
#include <map>

class Network
{
public:
	Network(std::map<std::string, std::string> config);
	bool StartListen();
	void StartHandleRequest();
	
private:
	std::map<std::string, std::string> settings;
	virtual std::map<std::string, std::string> ProtocolParser(std::string packet);
	
	virtual std::string EventHandler(std::string content);
};
