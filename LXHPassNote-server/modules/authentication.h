#include <string>

class Authentication
{
private:
	std::string mysqlHost;
	std::string mysqlUser;
	std::string mysqlPassword;

public:
	Authentication(std::string host, std::string user, std::string password) : mysqlHost(host), mysqlUser(user), mysqlPassword(password) {}
	~Authentication() {};

	std::string ToEmailAuthCode(std::string email);

	std::string SignUp(std::string password, std::string email, std::string authcode, std::string phone, std::string username);

	std::string SignIn(std::string username, std::string password);

	std::string UpdateUserDetails(std::string username, std::string token, std::string avatar, std::string phone);

	std::string UpdateUserPassword(std::string username, std::string newpassword, std::string authcode);

	std::string DeleteAccount();


};

