#include "auto_sign_in.h"
#include "HTTPRequest.h"
#include "md5.h"
#include "FileIO.h"

#include <regex>
#include <stdexcept>
#include <ctime>
#include <iostream>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

std::vector<std::string> SignIn::SendSignInRequest(std::string UserBDUSS, std::vector<BarInfo> kwList)
{
	std::vector<std::string> kwFailedList;
	for (auto item : kwList)
	{
		std::string currentTBS;
		try
		{
			currentTBS = GetTBS(UserBDUSS, item.forum_name);
		}
		catch (std::runtime_error error)
		{
			OutputExceptionMessager(error.what());
			kwFailedList.push_back(item.forum_name);
			continue;
		}

		try
		{
			sign(UserBDUSS, item.forum_id, item.forum_name, currentTBS);
		}
		catch (std::runtime_error error)
		{
			OutputExceptionMessager(error.what());
			kwFailedList.push_back(item.forum_name);
			continue;
		}
		sleep(2);
	}
	return kwFailedList;
}

/************************************************************************************************************************
*��ѯ�û���ע������
*������UserBDUSS                                  | �û���BDUSS
*���أ�std::vector<BarInfo>                      | �û�������kw��FID���б�
*************************************************************************************************************************/
std::vector<BarInfo> SignIn::GetUserILikeList(std::string userBDUSS)
{
	std::regex format("img forum-id=\"(.*?)\" forum=\"(.*?)\"");
	
	HTTPRequest request;
	std::vector<std::string> additionalHeaders;
	std::vector<unsigned char> data;
	additionalHeaders.push_back("Cookie: BDUSS=" + userBDUSS + "\r\n");
	additionalHeaders.push_back("User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/68.0.3440.106 Safari/537.36\r\n");
	additionalHeaders.push_back("Connection: close\r\n");
	if (!request.HTTPOpenRequest("http://tieba.baidu.com/i/i?fr=home", additionalHeaders, "GET", data,  80))
	{
		throw std::runtime_error("In SignIn::GetUserILikeList: HTTPOpenRequest failed with error string: " + request.lastErrorString);
	}
	const std::string responseHeaders = request.GetResponseHeaders();
	const std::string responseContent = request.GetResponseMessageBody();

	std::string statusCode = request.GetResponseStatusCode(responseHeaders);
	if (statusCode != "200")
	{
		throw std::runtime_error("In SignIn::GetUserILikeList: Server responsed status code: " + statusCode);
	}
	std::regex_iterator<std::string::const_iterator> begin(responseContent.cbegin(), responseContent.cend(), format);
	std::regex_iterator<std::string::const_iterator> end;
	std::vector<BarInfo> result;
	BarInfo temp;
	for (auto &index = begin; index !=end ; index++)
	{
		temp.forum_id=index->str(1);
		temp.forum_name = index->str(2);
		result.push_back(temp);
	}
	
	return result;
}

/************************************************************************************************************************
*��ȡĳ���ɵĵ�ǰtbs
*������kw                                      | ���������ڴ��뱾����ǰӦ�������url����
*���أ�std::string                            | tbs
*************************************************************************************************************************/
std::string SignIn::GetTBS(std::string UserBDUSS, std::string kw)
{
	std::string url = "http://tieba.baidu.com/mo/m?kw=" + kw;
	HTTPRequest request;
	
	std::vector<std::string> additionalHeaders;
	additionalHeaders.push_back("Cookie: BDUSS=" + UserBDUSS + "\r\n");
	additionalHeaders.push_back("Connection: close\r\n");
	additionalHeaders.push_back("User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/68.0.3440.106 Safari/537.36\r\n");
	std::vector<unsigned char> additionalData;
	if (!request.HTTPOpenRequest(url, additionalHeaders, "GET", additionalData, 80))
	{
		throw std::runtime_error("In SignIn::GetTBS: HTTPOpenRequest failed with error string : " + request.lastErrorString);
	}

	std::string responseHeaders = request.GetResponseHeaders();
	std::string responseContent = request.GetResponseMessageBody();
	std::string statusCode = request.GetResponseStatusCode(responseHeaders);
	if (statusCode != "200")
	{
		throw std::runtime_error("In SignIn::GetTBS: Server responsed status code: " + statusCode);
	}

	std::regex format("name=\"tbs\" value=\"(.*?)\"");
	std::smatch result;
	if (!std::regex_search(responseContent, result, format))
	{
		throw std::runtime_error("In SignIn::GetTBS: Unable to locate tbs");
	}

	return result[1];
}

/************************************************************************************************************************
*ǩ��ĳ������
*������UserBDUSS                                   | �û���BDUSS
	       fid                                                | ����FID
		   kw                                                | ����kw
           tbs                                                | ��ǩ�����ɵ�tbs
*���أ�bool                                              | ǩ���ɹ�����true ʧ�ܷ���false
*************************************************************************************************************************/
bool SignIn::sign(std::string UserBDUSS, std::string fid, std::string kw, std::string tbs)
{
	HTTPRequest request;

	std::vector<unsigned char> data = request.URLdecode(kw);
	std::string kw_urldecoded(data.begin(), data.end());
	std::string parameter = "BDUSS=" + UserBDUSS + "&fid=" + fid + "&kw=" + kw + "&tbs=" + tbs + "&sign=";
	std::string sign = "BDUSS=" + UserBDUSS + "fid=" + fid + "kw=" + kw_urldecoded + "tbs=" + tbs + "tiebaclient!!!";

	
	parameter+= MD5(sign).toStr();

	std::vector<std::string> additionalHeaders;
	additionalHeaders.push_back("Cookie: BDUSS=" + UserBDUSS + "\r\n");
	additionalHeaders.push_back("Connection: close");
	additionalHeaders.push_back("Content-Type: application/x-www-form-urlencoded\r\n");
	additionalHeaders.push_back("User-Agent: bdtb for Android 8.0\r\n");

	std::vector<unsigned char> additionalData(parameter.cbegin(), parameter.cend());

	std::string url = "http://c.tieba.baidu.com/c/c/forum/sign";
	if (!request.HTTPOpenRequest(url, additionalHeaders, "POST", additionalData, 80))
	{
		throw std::runtime_error("In SignIn::sign: HTTPOpenRequest failed with error string" + request.lastErrorString);
	}

	std::string responseHeaders = request.GetResponseHeaders();
	std::string responseContent = request.GetResponseMessageBody();
	std::string statusCode = request.GetResponseStatusCode(responseHeaders);
	if (statusCode != "200")
	{
		throw std::runtime_error("In SignIn::sign: response status code: " + statusCode);
	}
	
	std::regex format("\"error_code\":\"(.*?)\"");
	std::smatch result;
	if (!std::regex_search(responseContent, result, format))
	{
		throw std::runtime_error("In SignIn::sign: Failed to find error_code ");
	}
	
	//0-ǩ���ɹ� 160002-�Ѿ�ǩ��
	if (result[1] != "0" && result[1] != "160002")
	{
		throw std::runtime_error("In SignIn::sign: error_code is " + result[1].str() + "with response " + responseContent);
	}
	return true;
}

/************************************************************************************************************************
�����Ϣ��log�ļ���ȥ
*������what  | �������log�ļ��е���Ϣ
*���أ���
*************************************************************************************************************************/
void SignIn::OutputExceptionMessager(std::string what)
{
	SignInFileIO io;
	io.WriteLog(what);
	io.~SignInFileIO();
}

/************************************************************************************************************************
��ʼ�Զ�ǩ�����ӣ����ñ��������תΪ�ػ����̼�������
*��������
*���أ���
*************************************************************************************************************************/
void SignTask::StartWatching()
{
	SwitchToDaemonProcess();

	SignInFileIO io;
	time_t rawTime = 0;
	tm *currentTime = nullptr;
	while (true)
	{
		rawTime = time(NULL);
		currentTime = localtime(&rawTime);
		if (io.config.hour == currentTime->tm_hour)
		{
			if (io.config.minute == currentTime->tm_min)
			{
				io.WriteLog("Start sign in...");
				StartAutoSignIn();
			}
		}
		io.WriteLog("Process is running....");
		sleep(28);
	}
}

/************************************************************************************************************************
�л�Ϊ�ػ���������
*��������
*���أ���
*************************************************************************************************************************/
void SignTask::SwitchToDaemonProcess()
{
	__pid_t pid = fork();
	if (pid != 0)
	{
		exit(0);
	}
	umask(0);
	setsid();
	chdir("/");
}

/************************************************************************************************************************
�Զ�ǩ�����ݿ������еİٶ��˺ţ�������ݿ�����ʧ�ܳ����˳�
*��������
*���أ���
*************************************************************************************************************************/
void SignTask::StartAutoSignIn()
{
	SignInFileIO io;
	MySQLConnect session;
	if (!session.Connect(_info))
	{
		io.WriteLog("In SignTask::StartAutoSignIn(): " + session.lastErrorString);
		exit(0);
	}

	session.Query("USE SignInUserInfo");
	std::vector<std::vector<std::string> > baidu_user_info;
	unsigned long long linesCount = session.SelectData("BDUSS,TIEBA_USERNAME", "baidu_user_info", "", baidu_user_info);
	session.CloseConnection();
	SignIn request;
	for (unsigned long long id = 0; id < linesCount; id++)
	{
		std::vector<BarInfo> barList;
		try
		{
			barList = request.GetUserILikeList(baidu_user_info[id][0]);
		}
		catch (std::runtime_error error)
		{
			io.WriteLog(error.what());
			continue;
		 }
		std::vector<std::string> failedKwList = request.SendSignInRequest(baidu_user_info[id][0], barList);
		if (!failedKwList.empty())
		{
			std::string errorMessage = "User " + baidu_user_info[id][1] + "failed with kw(s): ";
			for (auto item : failedKwList)
			{
				errorMessage += item + " ";
			}
			io.WriteLog(errorMessage);
		}
	}
}

