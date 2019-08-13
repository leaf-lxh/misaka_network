## tinyLogger

轻量的日志记录服务，具备身份认证，SSL传输功能。

[文档 ](#doc/)  /  [源代码](#src/)



需要重构...对象结构比较混乱。目前仅仅能用



### 依赖

* 支持C++11的编译器
* cmake (版本要求大于3.5)



### 安装

1.  `$ git clone https://github.com/leaf-lxh/misaka_network.git`

2. 进入tinyLogger/build目录
3. 执行build.sh脚本进行编译，得到`tinyLogger`可执行文件
4. 执行conf.sh脚本进行安装



### 使用

使用root权限执行tinyLogger命令

```
tinyLogger {start|status|stop}

--start  开启服务

--status 查看服务状态

--stop   终止服务运行

```



### 配置

配置文件位于`/etc/tinyLogger/setting.conf`

