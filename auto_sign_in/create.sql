--创建数据库
create database if not exists SignInUserInfo character set utf8;

use SignInUserInfo;

create table if not exists baidu_user_info
(
	id int unsigned not null auto_increment primary key,
	BDUSS TEXT(300) not null,
	TIEBA_USERNAME char(20) not null,
	E_MAIL TEXT(200) 
);

--数据库管理用户的创建以及权限的设置
create user if not exists "请设置用户名"@"localhost" identified by "请设置密码";
grant select, delete, update, insert on SignInUserInfo.* to "请设置用户名"@"localhost";
flush privileges;

--添加数据
Insert into baidu_user_info(BDUSS, TIEBA_USERNAME, E_MAIL) values
("AAAAA", "用户名", "邮箱");
