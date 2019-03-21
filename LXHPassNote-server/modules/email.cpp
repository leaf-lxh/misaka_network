#include "email.h"

#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#include <openssl/ssl.h>

//function copied from https://github.com/lishuhuakai/Mail
char* base64Encode(char const* origSigned, unsigned origLength)
{
	static const char base64Char[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	unsigned char const* orig = (unsigned char const*)origSigned; // in case any input bytes have the MSB set
	if (orig == NULL) return NULL;

	unsigned const numOrig24BitValues = origLength / 3;
	bool havePadding = origLength > numOrig24BitValues * 3;
	bool havePadding2 = origLength == numOrig24BitValues * 3 + 2;
	unsigned const numResultBytes = 4 * (numOrig24BitValues + havePadding);
	char* result = new char[numResultBytes + 3]; // allow for trailing '/0'

												 // Map each full group of 3 input bytes into 4 output base-64 characters:
	unsigned i;
	for (i = 0; i < numOrig24BitValues; ++i)
	{
		result[4 * i + 0] = base64Char[(orig[3 * i] >> 2) & 0x3F];
		result[4 * i + 1] = base64Char[(((orig[3 * i] & 0x3) << 4) | (orig[3 * i + 1] >> 4)) & 0x3F];
		result[4 * i + 2] = base64Char[((orig[3 * i + 1] << 2) | (orig[3 * i + 2] >> 6)) & 0x3F];
		result[4 * i + 3] = base64Char[orig[3 * i + 2] & 0x3F];
	}

	// Now, take padding into account.  (Note: i == numOrig24BitValues)
	if (havePadding)
	{
		result[4 * i + 0] = base64Char[(orig[3 * i] >> 2) & 0x3F];
		if (havePadding2)
		{
			result[4 * i + 1] = base64Char[(((orig[3 * i] & 0x3) << 4) | (orig[3 * i + 1] >> 4)) & 0x3F];
			result[4 * i + 2] = base64Char[(orig[3 * i + 1] << 2) & 0x3F];
		}
		else
		{
			result[4 * i + 1] = base64Char[((orig[3 * i] & 0x3) << 4) & 0x3F];
			result[4 * i + 2] = '=';
		}
		result[4 * i + 3] = '=';
	}

	result[numResultBytes] = '\0';
	return result;
}


int SMTPComunicate(int connection, const SSLEmailService::EmailInfo &info, std::string &lastErrorString)
{
	char * buffer = new char[1000];
	memset(buffer, 0, 1000);

	recv(connection, buffer, 999, 0);
	if (strncmp(buffer, "220", 3) != 0) // not equal to 220
	{
		lastErrorString = buffer;
		return 220;
	}

	//向服务器发送ehlo
	std::string command = "ehlo EmailService\r\n";
	send(connection, command.c_str(), command.length(), 0);

	memset(buffer, 0, 1000);
	recv(connection, buffer, 999, 0);
	if (strncmp(buffer, "250", 3) != 0) // ehlo failed
	{
		lastErrorString = buffer;
		return 250;
	}

	//进行登录验证
	command = "AUTH PLAIN ";
	std::string auth = '\0' + info.senderEmail + '\0' + info.password;
	command += base64Encode(auth.data(), auth.size());
	command += "\r\n";
	send(connection, command.c_str(), command.length(), 0);

	memset(buffer, 0, 1000);
	recv(connection, buffer, 999, 0);
	if (strncmp(buffer, "235", 3) != 0) // login failed
	{
		lastErrorString = buffer;
		return 250;
	}

	//设置邮件发送者的邮箱地址
	command = "mail FROM:<" + info.senderEmail + ">\r\n";
	send(connection, command.c_str(), command.length(), 0);

	memset(buffer, 0, 1000);
	recv(connection, buffer, 999, 0);
	if (strncmp(buffer, "250", 3) != 0) // not ok
	{
		lastErrorString = buffer;
		return 250;
	}

	//设置邮件接收者的邮箱地址
	command = "rcpt TO:<" + info.recipientEmail + ">\r\n";
	send(connection, command.c_str(), command.length(), 0);

	memset(buffer, 0, 1000);
	recv(connection, buffer, 999, 0);
	if (strncmp(buffer, "250", 3) != 0) // not ok
	{
		lastErrorString = buffer;
		return 250;
	}

	//准备发送邮件
	//command.clear();
	command = "data\r\n";
	send(connection, command.c_str(), command.length(), 0);

	memset(buffer, 0, 1000);
	recv(connection, buffer, 999, 0);
	if (strncmp(buffer, "354", 3) != 0) // not ready to receive message
	{
		lastErrorString = buffer;
		return 354;
	}

	//设定邮件的发送者名称、接收者名称、邮件主题，邮件内容。

	command = "Content-Type: " + info.MIMEContentType + "; charset=\"" + info.charset + "\"\r\n";
	command += "MIME-Version: 1.0\r\n";
	command += "Content-Transfer-Encoding: base64\r\n";
	command += "From: =?" + info.charset + "?b?" + base64Encode(info.sender.c_str(), info.sender.length()) + "?= <" + info.senderEmail + ">\r\n";
	command += "To: =?" + info.charset + "?b?" + base64Encode(info.recipient.c_str(), info.recipient.length()) + "?= <" + info.recipientEmail + ">\r\n";
	command += "Subject: =?" + info.charset + "?b?" + base64Encode(info.subject.c_str(), info.subject.length()) + "?=\r\n";
	command += "\r\n";
	command += base64Encode(info.message.c_str(), info.message.length());
	command += "\r\n.\r\n";
	send(connection, command.c_str(), command.length(), 0);

	memset(buffer, 0, 1000);
	recv(connection, buffer, 999, 0);
	if (strncmp(buffer, "250", 3) != 0) // not ok
	{
		lastErrorString = buffer;
		return 250;
	}

	//结束发送过程
	delete buffer;
	send(connection, "quit\r\n", 6, 0);
	return 0;
}

int SMTPSSLComunicate(SSL *connection, const SSLEmailService::EmailInfo &info, std::string &lastErrorString)
{
	char * buffer = new char[1000];
	memset(buffer, 0, 1000);

	SSL_read(connection, buffer, 999);
	if (strncmp(buffer, "220", 3) != 0) // not equal to 220
	{
		lastErrorString = buffer;
		return 220;
	}

	//向服务器发送ehlo
	std::string command = "ehlo EmailService\r\n";
	SSL_write(connection, command.c_str(), command.length());

	memset(buffer, 0, 1000);
	SSL_read(connection, buffer, 999);
	if (strncmp(buffer, "250", 3) != 0) // ehlo failed
	{
		lastErrorString = buffer;
		return 250;
	}

	//进行登录验证
	command = "AUTH PLAIN ";
	std::string auth = '\0' + info.senderEmail + '\0' + info.password;
	command += base64Encode(auth.data(), auth.size());
	command += "\r\n";
	SSL_write(connection, command.c_str(), command.length());

	memset(buffer, 0, 1000);
	SSL_read(connection, buffer, 999);
	if (strncmp(buffer, "235", 3) != 0) // login failed
	{
		lastErrorString = buffer;
		return 250;
	}

	//设置邮件发送者的邮箱地址
	command = "mail FROM:<" + info.senderEmail + ">\r\n";
	SSL_write(connection, command.c_str(), command.length());

	memset(buffer, 0, 1000);
	SSL_read(connection, buffer, 999);
	if (strncmp(buffer, "250", 3) != 0) // not ok
	{
		lastErrorString = buffer;
		return 250;
	}

	//设置邮件接收者的邮箱地址
	command = "rcpt TO:<" + info.recipientEmail + ">\r\n";
	SSL_write(connection, command.c_str(), command.length());

	memset(buffer, 0, 1000);
	SSL_read(connection, buffer, 999);
	if (strncmp(buffer, "250", 3) != 0) // not ok
	{
		lastErrorString = buffer;
		return 250;
	}

	//准备发送邮件
	command = "data\r\n";
	SSL_write(connection, command.c_str(), command.length());

	memset(buffer, 0, 1000);
	SSL_read(connection, buffer, 999);
	if (strncmp(buffer, "354", 3) != 0) // not ready to receive message
	{
		lastErrorString = buffer;
		return 354;
	}

	//设定邮件的发送者名称、接收者名称、邮件主题，邮件内容。
	command = "Content-Type: " + info.MIMEContentType + "; charset=\"" + info.charset + "\"\r\n";
	command += "MIME-Version: 1.0\r\n";
	command += "Content-Transfer-Encoding: base64\r\n";
	command += "From: =?" + info.charset + "?b?" + base64Encode(info.sender.c_str(), info.sender.length()) + "?= <" + info.senderEmail + ">\r\n";
	command += "To: =?" + info.charset + "?b?" + base64Encode(info.recipient.c_str(), info.recipient.length()) + "?= <" + info.recipientEmail + ">\r\n";
	command += "Subject: =?" + info.charset + "?b?" + base64Encode(info.subject.c_str(), info.subject.length()) + "?=\r\n";
	command += "\r\n";
	command += base64Encode(info.message.c_str(), info.message.length());
	command += "\r\n.\r\n";
	SSL_write(connection, command.c_str(), command.length());

	memset(buffer, 0, 1000);
	SSL_read(connection, buffer, 999);
	if (strncmp(buffer, "250", 3) != 0) // not ok
	{
		lastErrorString = buffer;
		return 250;
	}

	//结束发送过程
	delete buffer;
	SSL_write(connection, "quit\r\n", 6);
	return 0;
}

int SSLEmailService::SendEmail(const SSLEmailService::EmailInfo &email, bool useSSL)
{
	//start socket connection
	int socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if (socketfd == -1)
	{
		this->lastErrorMsg = "Error on creating socket fd.";
		return -1;
	}

	addrinfo inAddrInfo = { 0 };
	inAddrInfo.ai_family = AF_INET;
	inAddrInfo.ai_socktype = SOCK_STREAM;

	addrinfo *addrInfo;
	if (getaddrinfo(email.smtpServer.c_str(), email.serverPort.c_str(), &inAddrInfo, &addrInfo) != 0) // error occurs
	{
		this->lastErrorMsg = "Error on calling getadrrinfo().";
		return -1;
	}


	if (connect(socketfd, addrInfo->ai_addr, addrInfo->ai_addrlen))
	{
		this->lastErrorMsg = "Error on calling connect().";
		return -1;
	}

	int retcode = 0;
	//decide whether to communicate under the SSL
	if (useSSL == true)
	{
		SSL_CTX *ctx;
		SSL *ssl;
		SSL_library_init();
		OpenSSL_add_all_algorithms();
		SSL_load_error_strings();
		ctx = SSL_CTX_new(SSLv23_client_method());

		ssl = SSL_new(ctx);
		SSL_set_fd(ssl, socketfd);
		SSL_connect(ssl);
		retcode = SMTPSSLComunicate(ssl, email, this->lastErrorMsg);

		SSL_shutdown(ssl);
		SSL_free(ssl);
		SSL_CTX_free(ctx);
	}
	else
	{
		retcode = SMTPComunicate(socketfd, email, this->lastErrorMsg);
	}

	freeaddrinfo(addrInfo);
	close(socketfd);
	return retcode;
}