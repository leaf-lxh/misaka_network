#include "auto_sign_in.h"
#include "HTTPRequest.h"
#include "md5.h"
#include <regex>


std::vector<std::string> SignIn::GetUserILikeList(std::string UserBDUSS)
{
	std::regex format("{\"user_id\":(.*?),\"forum_id\":(.*?),\"forum_name\":\"(.*?)\",\"is_like\":(.*?),\"is_black\":(.*?),\"like_num\":.*?,\"in_time\":.*?,\"level_id\":(.*?),\"cur_score\":(.*?),\"score_left\":(.*)?,\"level_name\":\"(.*?)\",\"is_sign\":(.*?)}");
	
	HTTPRequest request;
	std::vector<std::string> headers;
	std::vector<unsigned char> data;
	headers.push_back("Cookie: BDUSS=lRcE8zT0VhZkZCVmhmVVRzTH55QkVDZ3dLcTk1TDRSMjRqNVdJRG9qSHYtbU5iQVFBQUFBJCQAAAAAAAAAAAEAAAAojMdOxMe317fJtcTCzNK2AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAO9tPFvvbTxben\r\n");
	headers.push_back("User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/68.0.3440.106 Safari/537.36\r\n");
	if (!request.HTTPOpenRequest("http://tieba.baidu.com/i/i?fr=home", headers, "GET", data,  80))
	{
		return;
	}

	std::
	std::string statusCode(request.GetResponseHeaders().begin(), request.
	request.GetResponseMessageBody();
	return std::vector<std::string>();
}
