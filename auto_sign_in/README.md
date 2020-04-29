# auto_sign_in
   自动化百度贴吧签到

## Build from source:
本项目依赖以下软件:
- MySQL-server (version above 5.5)
- MySQL-client
- libmysqlclient-dev
- git
- cmake (version above 3.5)
- > Ubuntu用户可使用install-pre-environment.sh进行安装
```
    [1]克隆本仓库
    # git clone https://github.com/leaf-lxh/misaka_network.git
    # cd auto_sign_in

    [2]修改并执行create.sql，以设置数据库用户和签到用户凭证。
    # mysql -u account -p < create.sql

    [3]编译并运行
    # cmake .
    # make
    -------------------------初始化-------------------------
    数据库账号[root]
    密码:
    端口[3306]: 
    主机[127.0.0.1]
    开始监视，程序切换至后台运行

```
## 签到时间设定以及日志输出:
    配置文件: /etc/auto_sign_in/config.conf
    日志文件: /var/log/auto_sign_in/log-YYYY-MM-DD.log
    Log's path can be changed in the configuration file.
## To do:
> 常年深坑，有空再写
   - [ ] Encrypt user data that stored in the database.

   - [ ]  A managing web page.

   - [ ]  E-mail alert while failed to sign in.
  
   - [ ]  Support for windows.

