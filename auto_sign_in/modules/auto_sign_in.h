#include <string>
#include <vector>
#include "HTTPRequest.h"

struct BarInfo
{
	std::string forum_id;
	std::string forum_name;
};

class SignIn
{
public:

	//本类的构造函数
	SignIn() = default;
	//本类的析构函数
	~SignIn() = default;

	
	/************************************************************************************************************************
	*发送签到请求
	*参数：UserBDUSS                   | 将进行签到的用户的BDUSS
	*          kwList                          | 需要签到的贴吧列表
	*返回：std::vector<std::string> | 签到失败的贴吧kw列表
	*************************************************************************************************************************/
   std::vector<std::string> SendSignInRequest(std::string UserBDUSS, std::vector<BarInfo> kwList);

	/************************************************************************************************************************
	*查询用户关注的贴吧
	*参数：UserBDUSS                                  | 用户的BDUSS
	*返回：std::vector<BarInfo>                      | 用户的贴吧kw与FID的列表
	*************************************************************************************************************************/
	std::vector<BarInfo> GetUserILikeList(std::string UserBDUSS);

	inline std::string test(std::string UserBDUSS, std::string kw)
	{
		return GetTBS(UserBDUSS, kw);
	}
	
private:
	/************************************************************************************************************************
	*获取某贴吧的当前tbs
	*参数：kw                                      | 贴吧名，在传入本参数前应将其进行url编码
	*返回：std::string                            | tbs
	*************************************************************************************************************************/
    std::string GetTBS(std::string UserBDUSS, std::string kw);
    bool sign(std::string UserBDUSS, std::string fid, std::string kw, std::string tbs);
};