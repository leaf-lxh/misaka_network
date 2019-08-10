## 通信部分的实现文档

本文档描述了**tinyLogger**服务端的网络通信实现



### 协议

日志服务的通信基于TCP，并以其为基础，构建了一层自定义协议

自定义协议由两部分组成，报文头和报文体，报文头与报文体之间以两个**CR LF**(即\r\n)作为分隔符

报文头用于存储属性相关的信息，由以下字段构成，每个字段之间以一个**CR LF**作为分隔符

| 字段名      | 说明                   |
| ----------- | ---------------------- |
| version     | 日志服务程序的版本信息 |
| app_name    | 设施名称               |
| app_key     | 在服务端设置的设施密钥 |
| data_length | 数据长度               |

报文体用于存储数据，如日志数据。报文体的数据长度由报文头中的**data_length**字段决定，多出来的数据将会被丢弃



### 数据接收

考虑到并发连接量较少，服务端单进程使用select函数实现IO复用



### 实现

由Network类实现对外的网络服务

头文件：src/network.h

源文件：src/network.h

#### 整体执行流程

```flow
start=>start: 构造函数初始化Network对象设定
end=>end: 析构函数关闭当前所有已建立连接

startListen=>operation: Network::StartListen()
startProcess=>operation: Network::StartHandleRequest()

serverState=>condition: Network::listenState == Network::ListenState::RUN

start->startListen->startProcess->serverState
serverState(yes)->startProcess
serverState(no)->end
```



#### Network::Network()

Netwrok类的构造函数，用于配置信息的初始化。配置信息需包含将绑定的地址和端口。默认为**127.0.0.1:7200**



#### Network::~Network()

Network类的析构函数，用于关闭当前已建立的连接



#### Network::StartListen()

执行bind() listen()监听操作，并设置监听套接字为非阻塞。

如果中途发生错误会抛出std::runtime_error()异常，错误信息可调用异常对象的what方法来获取。



####  Network::StartHandleRequest()

类中有两个std::map类型的私有成员`connectionPool`和`connectionBuffer`，用于存储连接信息。

connectionPool将会存储每个连接的源IPv4地址和源端口信息。

connectionBuffer将存储每个连接接收到的字节流数据

每当一个连接的数据接收完后，将调用EventHandler函数，调用时会检查异常情况，如发生异常将关闭该连接

如果EventHandler返回空字符串，意味着其需要更多的数据，此时不会向客户端发送任何数据，直接进入下一个连接的处理

函数会一直检查私有成员`listenState`的数值，如果不是RUN状态将会终止循环。具体信息请查看实现



#### virtual Network::EventHandler()

请求处理函数。每当一个连接的数据接收完后，将以对应连接的`connectionBuffer.second`为参数调用本函数。

`connectionBuffer.second`是一个string对象，需要由本函数来进行缓冲区数据的维护，包括对缓冲区数据中可能存在多个报文的识别处理。

在处理完一个报文后应对其请求数据进行清除，因为如果不清除数据会一直存放于缓冲区。

 本函数的返回数据将作为返回数据发送给客户端。



#### Network::GracefullyShutdown()

用于将本类的

### 使用

以本类为基类，重载实现EventHandler()函数

