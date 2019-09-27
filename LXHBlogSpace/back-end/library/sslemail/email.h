#include <string>

#include <vector>

class SSLEmailService
{
public:
	///调用函数失败时会将错误原因赋值到此字符串中
	std::string lastErrorMsg;
	struct EmailInfo
	{
		std::string smtpServer;      //the SMTP server
		std::string serverPort;      //the SMTP server port
		std::string charset;         //the email character set
		std::string sender;          //the sender's name
		std::string senderEmail;     //the sender's email
		std::string password;        //the password of sender
		std::string recipient;       //the recipient's name
		std::string recipientEmail;  //the recipient's email

		std::string MIMEContentType; //the message's content type;
		std::string subject;         //the email message's subject
		std::string message;         //the email message body

	}email;

	/*!
	发送邮件
	参数：email  | 邮件的信息
	参数：useSSL | 是否使用SSL进行通信
	返回：发送成功返回0，失败时返回大于0的SMTP错误码，或-1，创建连接失败
	*/
	int SendEmail(bool useSSL);
};
