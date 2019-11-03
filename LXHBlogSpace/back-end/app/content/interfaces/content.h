#include <tinyHttpd/tinyHttpd.h>

#include <tinyHttpd/HTTPPacket.h>

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>

class BlogSpaceContent : public TinyHttpd
{
public:
	void ModuleInit() noexcept(false);


private:
	///MYSQL服务的属性
	struct MySQLProperty
	{
		//ip:port类型
		std::string host;
		std::string user;
		std::string password;
		sql::Driver* driverInstance;
		std::unique_ptr<sql::Connection> connection;
	}mysqlProperty;
	
	std::string imageRootPath;

	/*!
	仅初始化MYSQL数据库连接。如需重新连接请使用reconnect方法
	异常：初始化失败（如无法连接数据库，账密错误等），抛出runtime_error
	*/
	void InitMysqlConnection() noexcept(false);

	///MySQL对象指针的包装
	using PtrPreparedStatement = std::unique_ptr<sql::PreparedStatement>;
	using PtrStatement = std::unique_ptr<sql::Statement>;
	using PtrResultSet = std::unique_ptr<sql::ResultSet>;

	/*!
	本服务的HTTP报文处理器
	*/
	void HTTPPacketHandler(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept;

	/****************外部公开接口***************/
	HTTPPacket::HTTPResponsePacket UploadImage(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false);
	HTTPPacket::HTTPResponsePacket SaveDraft(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false);
	HTTPPacket::HTTPResponsePacket ReadDraft(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false);
	HTTPPacket::HTTPResponsePacket DeleteDraft(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false);
	HTTPPacket::HTTPResponsePacket GetDraftList(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false);
	
	HTTPPacket::HTTPResponsePacket PublishArticle(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false);
	HTTPPacket::HTTPResponsePacket GetArticleContent(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false);
	HTTPPacket::HTTPResponsePacket DeleteArticle(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false);
	HTTPPacket::HTTPResponsePacket GetUserArticleList(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false);

	HTTPPacket::HTTPResponsePacket Vote(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false);
	HTTPPacket::HTTPResponsePacket Subscribe(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false);

	HTTPPacket::HTTPResponsePacket GetSubscribedList(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false);
	HTTPPacket::HTTPResponsePacket CheckUserOperation(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false);

	HTTPPacket::HTTPResponsePacket SendComment(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false);
	HTTPPacket::HTTPResponsePacket GetComments(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false);

	HTTPPacket::HTTPResponsePacket GetPublishArticleList(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false);
	HTTPPacket::HTTPResponsePacket GetFollowedArticleList(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false);
	HTTPPacket::HTTPResponsePacket GetHottopic(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false);

	HTTPPacket::HTTPResponsePacket GetFriendLink(int clientfd, HTTPPacket::HTTPRequestPacket request) noexcept(false);

	/*****************内部调用接口**************/
	/*!
	验证用户登录是否有效，并且未被封禁。
	参数：request | 当前用户的请求报文
	返回：符合条件为true, 否则为false
	异常：执行SQL语句时如遇错误会抛出sql::SQLException
	*/
	bool CheckUserToken(HTTPPacket::HTTPRequestPacket &request) noexcept(false);

	/*!
	准备文件存储的路径，此函数保证返回的文件路径唯一，不会覆写其他文件
	返回：相对于imageRootDir的存储路径
	异常：如果尝试10次生成仍能无法生成一个唯一的路径，抛出std::runtime_error
	*/
	std::string PrepareDirectory(std::string fileExtention) noexcept(false);

	/*!
	根据用户名获取其uuid。
	异常：可能会抛出sql::SQLException
	*/
	std::string GetUUIDByUsername(std::string uuid) noexcept(false);

	/*!
	根据用户uuid获取其头像
	异常：可能会抛出sql::SQLException
	*/
	std::string GetUserAvatar(std::string uuid) noexcept(false);

	/*!
	根据用户uuid获取其用户名
	异常：可能会抛出sql::SQLException
	*/
	std::string GetUsernameByUUID(std::string username) noexcept(false);

	/*!
	获取文章的评论数
	异常：可能会抛出sql::SQLException
	*/
	int GetArticleCommentNum(std::string articleId) noexcept(false);

	/*!
	获取文章的标题
	异常：可能会抛出sql::SQLException
	*/
	std::string GetArticleTitle(std::string articleId) noexcept(false);

	/*!
	获取文章的作者uuid
	异常：可能会抛出sql::SQLException
	*/
	std::string GetArticleAuthor(std::string articleId) noexcept(false);

	/*!
	查询指定文章是否存在。
	异常：可能会抛出sql::SQLException
	*/
	bool IsArticleExist(std::string aritcleId) noexcept(false);
	/*!
	查询用户是否对指定文章进行过赞同操作
	异常：可能会抛出sql::SQLException
	*/
	bool IsVoted(std::string uuid, std::string articleId) noexcept(false);

	/*!
	查询用户是否对指定文章进行过收藏操作
	异常：可能会抛出sql::SQLException
	*/
	bool IsSubscribed(std::string uuid, std::string articleId) noexcept(false);

	std::string MarkdownBriefPretty(std::string text) noexcept;
};
