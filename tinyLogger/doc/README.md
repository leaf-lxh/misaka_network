## tinyLogger

此处存放着本日志服务的说明文档



### 目录

* [配置说明](#conf.md)
* [通信部分的实现](#network.md)
* [日志服务的实现](#logger.md)



tinyLogger采用master-worker的工作方式，由主进程进行工作进程的创建和维护。

工作进程负责网络通信和数据的处理。