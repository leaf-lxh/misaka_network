#include "auto_sign_in.h"
#include "HTTPRequest.h"
#include "md5.h"
#include <regex>
#include <stdexcept>

/************************************************************************************************************************
*查询用户关注的贴吧
*参数：UserBDUSS                                  | 用户的BDUSS
*返回：std::vector<BarInfo>                      | 用户的贴吧kw与FID的列表
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
*获取某贴吧的当前tbs
*参数：kw                                      | 贴吧名，在传入本参数前应将其进行url编码
*返回：std::string                            | tbs
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

bool SignIn::sign(std::string UserBDUSS, std::string fid, std::string kw, std::string tbs)
{
	return false;
}
