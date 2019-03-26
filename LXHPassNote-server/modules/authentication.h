#include <string>

class Authentication
{
private:
	std::string mysqlHost;
	std::string mysqlUser;
	std::string mysqlPassword;

public:
	Authentication(std::string sqlhost, std::string sqluser, std::string sqlpassword) : mysqlHost(sqlhost), mysqlUser(sqluser), mysqlPassword(sqlpassword) {}
	~Authentication() {};
	
	///错误码, 本类所有的函数在发生错误时都会设置该错误码
	std::string lastErrorCode;

	///错误信息，部分函数会在发生错误时设置错误信息。设置错误信息的函数会在函数声明的注释中注明
	std::string lastErrorMsg;

	/*!
	发送验证码到指定的邮箱，验证码同时会储存至数据库
	参数：email         | 指定的邮箱，长度应小于40（不包含\0）
	参数：smtpserver    | smtp服务器域名或IPv4地址
	参数：smtpport      | smtp服务端口
	参数：emailAccount  | 登录smtp服务器时使用的邮箱地址
	参数：emailPassword | 登录smtp服务器时使用的密码
	返回：注册验证码，失败返回空字符串
	注意：如果是调用connector时发生异常而导致的失败，会将SQL错误码保存到lastErrorMsg。当邮件发送失败时，会将SMTP服务器返回的错误信息保存到lastErrorMsg
	*/
	std::string ToEmailAuthCode(std::string email, std::string smtpserver, std::string smtpport, std::string emailAccount, std::string emailPassword);

	/*!
	进行注册
	参数：passwrod | 用户注册的密码，密码要求至少8位，包含数字、字母，特殊符号的其中两个
	参数：email    | 用户注册时提交的验证邮箱，长度应小于40（不包含\0）
	参数：authcode | 用户注册时提交的邮箱验证码
	参数：phone    | 用户注册时提交的手机号，可提交空字符串
	参数：username | 用户名，不可包含@
	返回：用户ID，失败返回空字符串
	*/
	std::string SignUp(std::string password, std::string email, std::string authcode, std::string phone, std::string username);

	/*!
	登录
	参数：username | 用户名或邮箱
	参数：password | 用户的密码
	返回：登录成功返回token，失败返回空字符串
	*/
	std::string SignIn(std::string username, std::string password);

	/*!
	更新用户详细资料
	参数：id       | 账户ID
	参数：token    | 用户凭证
	参数：username | 设定的用户名
	参数：avatar   | 设定的头像名称
	参数：phone    | 设定的手机号
	返回：成功返回true, 失败返回false
	*/
	bool UpdateUserDetails(std::string id, std::string token, std::string username, std::string avatar, std::string phone);

	/*!
	更新用户密码，如果成功会删除该用户的所有凭证
	参数：id           | 用户ID
	参数：authcode     | 邮件验证码
	参数：newpassword  | 新的密码
	返回：成功返回true, 失败返回false
	*/
	bool UpdateUserPassword(std::string id, std::string authcode, std::string newpassword);

	/*!
	根据给予的邮箱地址查询用户ID
	参数：email | 用户邮箱
	返回：成功返回用户ID，失败返回空字符串
	*/
	std::string GetIDByEmail(std::string email);
};

