# Module Access Control

文档版本 1.0 2019-3-14

本文档描述了用户访问控制(Access Control)模块的实现

### 目录

* [用户信息表(user_info)](#用户信息表(user_info))
* [用户详细信息表(user_details)](#用户详细信息表(user_details))
* [用户认证信息表(user_auth)](#用户认证信息表(user_auth))

### 环境

用户认证数据存储在MySQL数据库中.可自定义用户数据库名称.默认名称为lxhpassnote_user

### 用户信息表(user_info)

```mysql
用户信息表,包含用户ID, 用户锁定状态.此表仅用于保存用户存在信息,为主表
create table user_info(
    user_id int unsigned  not null auto_increment primary key,
    is_locked bit not null
)ENGINE=InnoDB DEFAULT CHARSET=utf8;
    
```

| 字段名    | 例子         | 说明                           |
| --------- | ------------ | ------------------------------ |
| user_id   | 1            | 用户的ID                       |
| is_locked | int not null | 用户锁定状态,1为锁定,0为未锁定 |

### 用户详细信息表(user_details)

```mysql
用户详细信息表，包含用户ID，用户名称，用户头像图片名称，用户手机号，用户邮箱，用户注册时间
create table user_details(
	user_id int unsigned not null,
    user_name text(30) not null,
    user_avatar text(50) not null,
    user_phone text(20) not null,
    user_email text(25) not null,
    user_signup_date text(30) not null
)ENGINE=InnoDB DEFAULT CHARSET=utf8;
```



| 字段名           | 例子                | 说明                            |
| ---------------- | ------------------- | ------------------------------- |
| user_id          | 1                   | 用户ID，应与user_info中的ID对应 |
| user_name        | test_user           | 用户名称                        |
| user_avatar      | SHA1.png            | 用户头像图片名称                |
| user_phone       | +8613123456789      | 用户手机号                      |
| user_email       | test_user@test_name | 用户邮箱                        |
| user_signup_date | 1552567132          | 用户注册时间Unix时间戳          |

### 用户认证信息表(user_auth)

```mysql
用户认证信息表，包含用户ID，用户密码，数据摘要运算使用的盐，用户权限标识
create table user_auth(
	user_id int unsigned not null,
    user_pass text(50) not null,
    user_salt text(30) not null,
    user_token text(200) not null,
    user_token_gen_date text(30) not null
)ENGINE=InnoDB DEFAULT CHARSET=utf8;
```

| 字段名              | 例子                                     | 说明                            |
| ------------------- | ---------------------------------------- | ------------------------------- |
| user_id             | 1                                        | 用户ID，应与user_info中的ID对应 |
| user_pass           | 7de05d05cddab2799fa7608ea4533002a95038b9 | 用户密码+盐的数据摘要           |
| user_salt           | 1552567183abcde                          | 盐                              |
| user_token          | abcdef                                   | 用户权限标识                    |
| user_token_gen_date | 1552567183                               | 用户权限标识创建时间            |

