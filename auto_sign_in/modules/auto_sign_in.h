#include <string>

class SignIn
{
public:
    //签到所有贴吧，返回签到失败数量
    int SendSignInRequest(std::string BDUSS,std::string STOKEN);

private:
    std::string BDUSS;
    std::string STOKEN;
    int GetILikeTiebaList();
    bool GetFID(std::string kw);
    bool GetTBS(std::string kw);
    bool sign(std::string kw);
};