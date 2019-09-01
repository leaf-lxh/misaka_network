/*
 * xhttp module, provides a http server framework.
*/

#pragma once

#include "parser/protocol.h"
#include "parser/setting.h"

#include "network/log.h"

#include <memory>
#include <vector>

namespace xhttp
{
	class EventHandler
	{
	public:
		EventHandler() = default;
	
		/*!
		HTTP事件的处理器
		param: headers | HTTP请求行和请求头
		param: message | HTTP的请求消息体
		retn:  对此次请求的回应数据
		*/
		virtual std::string Handler(HTTPHeadersType headers, std::string message);
	};

	class XHTTPServer
	{
	public:
		/*!
		构造函数，初始化根配置文件的绝对路径，以及unix域套接字的存放路径
		*/
		XHTTPServer(std::string settingPath = "/etc/xhttp/xhttp.conf", std::string unixSocketRootPath="/var/run/xhttp/");

		/*!
		进行静态资源服务器的初始化操作, 包含监听地址，根据配置文件初始化日志服务等
		retn: 成功返回0，失败返回错误码
		*/
		int InitDefaultServer();

		/*!
		将路径与事件处理器进行绑定，路由模式只在微服务模式下被使用
		param: path    | 路径，应以斜线(/slash）开头，末尾的/将会被忽略
		param: handler | 事件处理器，应使用make_shared进行生成
		retn:  成功返回0，失败返回错误码
		*/
		int AddRoute(std::string path, std::shared_ptr<EventHandler> handler);

		/*!
		进行微服务模式服务器的初始化操作
		retn: 成功返回0，失败返回错误码
		*/
		int InitServiceServer();

		/*!
		开始处理服务端的网络请求，包含工作模式的处理等
		*/
		void StartHandleRequest();
		 
	private:
		/*服务器模式，1为静态资源服务器模式，2为微服务模式*/
		enum class ServerMode : unsigned char { staticServer = 1, serviceServer = 2 } serverMode;

		/*服务器根配置文件的绝对路径*/
		std::string mSettingPath;

		/*用于进程间通信的unix域套接字文件的存放根路径*/
		std::string mUnixSocketRootPath;

		/*
		 * 微服务的路由表
		 * 每个路由由两个数据组成，第一个是路径，第二个是事件处理器。事件处理器应由make_shared来进行创建
		 */
		std::map<std::string, std::shared_ptr<EventHandler> > mRouteTable;


		/*
		 * 虚拟站点的属性结构
		 * 本结构由主进程进行维护
		 */
		struct VirtualHostProperty
		{
			/*该虚拟站点配置文件的绝对路径*/
			std::string settingPath;
			/*该虚拟站点的日志服务*/
			LogService* logService;

			/*该站点的域名*/
			std::string hostName;
			/*该站点的文件目录*/
			std::string documentRootDir;
			/*该虚拟站点的IPv4绑定地址和端口*/
			std::pair<std::string, std::string> strAddress;
			/*该虚拟站点的已连接客户端*/
			std::map<int, std::pair<std::string, std::string> > connectionInfoPool;
			/*该虚拟站点的已连接客户端的数据缓冲区*/
			std::map<int, std::pair<std::string, std::string> > connectionBufferPool;
		};

		/*当前的虚拟站点，键为主机名，值为虚拟站点属性*/
		std::map<std::string, VirtualHostProperty> mVirtualHosts;

		/*当前虚拟站点的监听套接字，允许多个虚拟站点共用一个监听套接字*/
		std::multimap<int, std::string> mVirtualHostsFD;

		/*!
		根据错误码返回错误信息
		param: code | 错误码
		retn:  返回此错误码对应的错误信息
		*/
		std::string GetErrorMsg(int code);
	};

}