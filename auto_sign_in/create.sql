create database if not exists SignInUserInfo character set utf8;

use SignInUserInfo;

create table if not exists baidu_user_info
(
	id int unsigned not null auto_increment primary key,
	BDUSS TEXT(300) not null,
	TIEBA_USERNAME char(20) not null,
	E_MAIL TEXT(200) 
);
