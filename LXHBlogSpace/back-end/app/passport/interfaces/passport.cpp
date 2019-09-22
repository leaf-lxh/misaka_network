#include "passport.h"
#include <sstream>
#include <iostream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#define SERVER_SIGNATURE "tinyHttpd/0.1a"

void BlogSpacePassport::InitRouteTabel() noexcept
{
	AddRoutePath("/api/v1/passport/");
	AddRoutePath("/api/v1/passport/login");
	AddRoutePath("/api/v1/passport/register");
	AddRoutePath("/api/v1/passport/GetUserInfo");
}

void BlogSpacePassport::HTTPPacketHandler(int clientfd, TinyHttpd::HTTPRequestPacket request) noexcept
{
	std::cout << request.requestPath << std::endl;

	if (serverProperty.routeTable.count(request.requestPath) == 0)
	{
		RaiseHTPPError(clientfd, 404);
		return;
	}

	TinyHttpd::HTTPResponsePacket response;
	if (request.requestPath == "/api/v1/passport/GetUserInfo")
	{
		response = GetUserInfo(clientfd, request);
		connectedClients[clientfd].writeBuffer += ResponseToString(response);
		return;
	}

	RaiseHTPPError(clientfd, 501);
}

TinyHttpd::HTTPResponsePacket BlogSpacePassport::GetUserInfo(int clientfd, TinyHttpd::HTTPRequestPacket request) noexcept(false)
{
	HTTPResponsePacket response;
	response.code = "200 OK";
	response.version = "HTTP/1.1";
	response.responseHeaders.insert({ "Server", SERVER_SIGNATURE });
	std::stringstream param;
	param << std::noskipws;
	param << "timeout=" << serverProperty.timeout << ", max=" << serverProperty.maxRequestsNum;
	response.responseHeaders.insert({ "Connection", "Keep-Alive" });
	response.responseHeaders.insert({ "Keep-Alive", param.str() });


	response.body = "{\"vaild\": true, \"name\": \"LegendLXH\", \"avatar\": \"https://blog.leaflxh.com/lxhcat.jpg\"}";
	response.responseHeaders.insert({ "Content-Encoding", "plain" });
	response.responseHeaders.insert({ "Content-Length", std::to_string(response.body.length()) });
	response.responseHeaders.insert({ "Content-Type", "text/plain; charset=utf8" });
	return response;
}