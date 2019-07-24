## httpserver core structure

### main.cpp

由main.cpp定义程序入口函数。

入口函数用于服务器的初始化过程，包括读取配置文件，各服务对象的创建及初始化。



### setting.h

读取配置文件，提供配置信息



### network.h

服务器的核心部分，提供网络通信的服务



### parser.h

处理HTTP报文，构建响应报文。包含对静态资源和API请求的处理



### gateway.h

提供反向代理服务，负载均衡服务，以及与API服务的通信





