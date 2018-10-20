# auto_sign_in
    
   From now, you can sign in Baidu tieba automaticly.

## Install:
Before installing, you need these softwares:
- MySQL-server (version above 5.5)
- MySQL-client
- libmysqlclient-dev
- git
- cmake (version above 3.5)
```
    [1]Clone the source code.
    # git clone https://github.com/leaf-lxh/misaka_network.git
    # cd auto_sign_in

    [2]Baidu user info setting.
    # mysql -u account -p < create.sql
    mysql> use SignInUserInfo;
    mysql> insert into baidu_user_info (BDUSS, TIEBA_USERNAME, E_MAIL) values("BDUSS1","NAME1","EMAIL1");
    mysql> insert into baidu_user_info (BDUSS, TIEBA_USERNAME, E_MAIL) values("BDUSS2","NAME2","EMAIL2");
    ...
    mysql> \q

    [3]Complie and run.
    # cmake .
    # make
    
    [4]Configure your mysql connecting info.
    # ./auto_sign_in
    -------------------------初始化-------------------------
    数据库账号[root]
    密码:
    端口[3306]: 
    主机[127.0.0.1]
    开始监视，程序切换至后台运行
```
## Configuration and log file:
    configuration: /etc/auto_sign_in/config.conf
    log file: /var/log/auto_sign_in/log-YYYY-MM-DD.log
    Log's path can be changed in the configuration file.
## To do:
  * Encrypt user data that stored in the database.

  * A HTML+JavaScript website to manage this program.

  * E-mail alert while failed to sign in.
  
  * Support for windows.

