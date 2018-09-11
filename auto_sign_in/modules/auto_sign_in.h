#include <string>
#include <vector>
#include "HTTPRequest.h"

class SignIn
{
public:
	SignIn(std::string BDUSS, std::string STOKEN);
	std::string BDUSS;
	std::string STOKEN;
    //签到所有贴吧，
    void SendSignInRequest();

	//查询未签到的贴吧,返回kw列表
	std::vector<std::string> GetUnsignedList();

private:
    
    int GetILikeTiebaList();
    bool GetFID(std::string kw);
    bool GetTBS(std::string kw);
    bool sign(std::string kw);
};