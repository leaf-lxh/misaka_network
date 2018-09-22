#include "auto_sign_in.h"
#include "HTTPRequest.h"
#include "md5.h"
#include <regex>
#include <stdexcept>


std::vector<BarInfo> SignIn::GetUserILikeList(std::string userBDUSS)
{
	std::regex format("\\u007B\"user_id\":.*?,\"forum_id\":(.*?),\"forum_name\":\"(.*?)\",\"is_like\":(.*?),\"is_black\":(.*?),\"like_num\":.*?,\"in_time\":.*?,\"level_id\":(.*?),\"cur_score\":(.*?),\"score_left\":(.*)?,\"level_name\":\"(.*?)\",\"is_sign\":(.*?)\\u005B");
	
	HTTPRequest request;
	std::vector<std::string> additionalHeaders;
	std::vector<unsigned char> data;
	additionalHeaders.push_back("Cookie: BDUSS=" + userBDUSS + "\r\n");
	additionalHeaders.push_back("User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/68.0.3440.106 Safari/537.36\r\n");
	if (!request.HTTPOpenRequest("http://tieba.baidu.com/i/i?fr=home", additionalHeaders, "GET", data,  80))
	{
		throw std::runtime_error("HTTPOpenRequest failed with error code: " + request.lastErrorString);
	}
	const std::string responseHeaders = request.GetResponseHeaders();
	const std::string responseContent = request.GetResponseMessageBody();

	std::string statusCode = request.GetResponseStatusCode(responseHeaders);
	if (statusCode != "200")
	{
		throw std::runtime_error("Server responsed status code: " + statusCode);
	}

	std::regex_iterator<std::string::const_iterator> begin(responseContent.cbegin(), responseContent.cend(), format);
	std::vector<BarInfo> result;
	BarInfo temp;
	for (auto index = begin; index != std::sregex_iterator(); index++)
	{
		temp.forum_id=index->str(1);
		temp.forum_name = index->str(2);
		temp.is_like = index->str(3);
		temp.is_black = index->str(4);
		temp.level_id = index->str(5);
		temp.cur_score = index->str(6);
		temp.score_left = index->str(7);
		temp.level_name = index->str(8);
		temp.is_sign = index->str(9);
	}
	
	return result;
}
