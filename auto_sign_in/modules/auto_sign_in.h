#include <string>
#include <vector>
#include "HTTPRequest.h"

class SignIn
{
public:

	//本类的构造函数
	SignIn(std::string BDUSS, std::string STOKEN);
	//本类的析构函数
	~SignIn();

	
	/************************************************************************************************************************
	*发送签到请求
	*参数：UserBDUSS                   | 将进行签到的用户的BDUSS
	*          kwList                          | 需要签到的贴吧列表
	*返回：std::vector<std::string> | 签到失败的贴吧kw列表
	*************************************************************************************************************************/
   std::vector<std::string> SendSignInRequest(std::string UserBDUSS, std::vector<std::string> kwList);

	/************************************************************************************************************************
	*查询用户关注的贴吧
	*参数：UserBDUSS                                  | 用户的BDUSS
	*返回：std::vector<string>                      | 用户关注的贴吧的列表
	*************************************************************************************************************************/
	std::vector<std::string> GetUnsignedList(std::string UserBDUSS);
	
private:
    
    
    bool GetFID(std::string kw);
    bool GetTBS(std::string kw);
    bool sign(std::string kw);
};