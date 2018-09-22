#include <string>
#include <vector>
#include "HTTPRequest.h"

struct BarInfo
{
	std::string forum_id;
	std::string forum_name;
	std::string is_like;
	std::string is_black;
	std::string level_id;
	std::string cur_score;
	std::string score_left;
	std::string level_name;
	std::string is_sign;
};

class SignIn
{
public:

	//本类的构造函数
	SignIn() = default;
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
	*返回：std::vector<string>                      | 用户关注贴吧的信息的列表
	*************************************************************************************************************************/
	std::vector<BarInfo> GetUserILikeList(std::string UserBDUSS);
	
private:
    bool GetTBS(std::string kw);
    bool sign(std::string kw);
};