# Module Access Control

文档版本 1.0 2019-3-14

本文档描述了用户访问控制(Access Control)模块的实现

头文件：authentication.h

​		email.h

源文件：authentication.cpp

​		email.cpp

### 目录

* [用户信息表(user_info)](#用户信息表(user_info))
* [用户详细信息表(user_details)](#用户详细信息表(user_details))
* [用户认证信息表(user_auth)](#用户认证信息表(user_auth))
* [注册认证表(signup_auth)](#注册认证表(signup_auth))
* [邮件发送](#邮件发送)
* [用户注册](#用户注册)
* [用户登录](#用户登录)
* [用户找回密码](#用户找回密码)
* [用户删除账号](#用户删除账号)

### 环境

用户认证数据存储在MySQL数据库中. 

```mysql
create database lxhpassnote_user;
```



### 用户信息表(user_info)

```mysql
用户信息表,包含用户ID,用户密码，数据摘要运算使用的盐，用户锁定状态.
create table user_info(
    user_id int unsigned  not null auto_increment primary key,
    user_pass tinytext not null,
    user_salt tinytext not null,
    is_locked int not null
)ENGINE=InnoDB DEFAULT CHARSET=utf8;
```

| 字段名    | 例子         | 说明                           |
| --------- | ------------ | ------------------------------ |
| user_id   | 1            | 用户的ID                       |
| user_pass           | 7de05d05cddab2799fa7608ea4533002a95038b9 | 用户密码+盐的数据摘要           |
| user_salt           | 1552567183abcde                          | 盐                              |
| is_locked | int not null | 用户锁定状态,1为锁定,0为未锁定 |

### 用户详细信息表(user_details)

```mysql
用户详细信息表，包含用户ID，用户名称，用户头像图片名称，用户手机号，用户邮箱，用户注册时间
create table user_details(
	user_id int unsigned not null,
    user_name varchar(30) not null,
    user_avatar varchar(50) not null,
    user_phone varchar(20) not null,
    user_email varchar(40) not null,
    user_signup_date varchar(30) not null
)ENGINE=InnoDB DEFAULT CHARSET=utf8;
```



| 字段名           | 例子                | 说明                                   |
| ---------------- | ------------------- | -------------------------------------- |
| user_id          | 1                   | 用户ID，应与user_info中的ID对应        |
| user_name        | test_user           | 用户名称，应确保唯一                   |
| user_avatar      | crc32_unix时间戳    | 二压后的用户头像文件名                 |
| user_phone       | +8613123456789      | 用户手机号                             |
| user_email       | test_user@test_name | 用户邮箱，应保证每个邮箱只绑定一个账号 |
| user_signup_date | 1552567132          | 用户注册时间Unix时间戳                 |

### 用户认证信息表(user_auth)

```mysql
用户认证信息表，包含用户ID，用户权限标识
create table user_auth(
	user_id int unsigned not null,
    user_token varchar(200) not null,
    user_token_gen_date varchar(30) not null
)ENGINE=InnoDB DEFAULT CHARSET=utf8;
```

| 字段名              | 例子                                     | 说明                            |
| ------------------- | ---------------------------------------- | ------------------------------- |
| user_id             | 1                                        | 用户ID，应与user_info中的ID对应 |
| user_token          | abcdef                                   | 用户权限标识                    |
| user_token_gen_date | 1552567183                               | 用户权限标识创建时间            |

### 注册认证表(signup_auth)

```mysql
注册认证表，用于存储注册时用于验证的验证码
create table signup_auth(
	user_email varchar(40) not null,
	user_phone varchar(20) not null,
	user_auth_code varchar(20) not null,
    create_time varchar(30) not null
)ENGINE=InnoDB DEFAULT CHARSET=utf8;
```

| 字段名         | 例子             | 说明                   |
| -------------- | ---------------- | ---------------------- |
| user_email     | test@example.com | 用户注册时提交的邮箱   |
| user_phone     | 131234564789     | 用户注册时提交的手机号 |
| user_auth_code | 123456           | 注册时系统生成的验证码 |
| create_time    | 1552567183       | 此验证码创建时间       |



### 邮件发送

认证时需要进行验证，暂时只考虑邮件验证码验证。



### 用户注册

首先调用





### 用户登录



