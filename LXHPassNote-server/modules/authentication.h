#include <string>

class Authentication
{
private:
	std::string mysqlHost;
	std::string mysqlUser;
	std::string mysqlPassword;

public:
	Authentication(std::string host, std::string user, std::string password) : mysqlHost(host), mysqlUser(user), mysqlPassword(password) {}
	~Authentication() {}

	

	std::string SignUp(std::string password, std::string email, std::string phone, std::string username);

	std::string SignIn(std::string username, std::string password);

	std::string UpdateUserDetails(std::string username, std::string avatar, std::string phone, );

	std::string DeleteAccount();


};

