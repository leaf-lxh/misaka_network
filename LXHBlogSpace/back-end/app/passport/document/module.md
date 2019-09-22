## passport

2019-07-09

ver 1.0

### 用户信息表(user_info)

用户信息表为后台信息数据的主体之一

```mysql
create table user_info(
    user_id int unsigned  not null auto_increment primary key,
    register_date int unsigned not null,
    is_locked int not null
)ENGINE=InnoDB DEFAULT CHARSET=utf8;
```

| 字段名        | 说明                           | 数据格式                   |
| ------------- | ------------------------------ | -------------------------- |
| user_id       | 用户id                         | 整数型数值，如1，2，3      |
| register_date | 用户注册时间                   | 整数型unix时间戳，精确到秒 |
| is_locked     | 是否被锁定，1为锁定，0为未锁定 | 整数型数值                 |

### 用户详细信息表(user_details)

用户详细信息表存储着用户的个人资料。为了减轻数据库的负担，数据约束上在应用层上做

```mysql
create table user_details(
	user_id int unsigned not null,
    username tinytext not null,
    avatar tinytext not null,
    email tinytext not null
)ENGINE=InnoDB DEFAULT CHARSET=utf8;
```

| 字段名   | 说明                          | 数据格式                      |
| -------- | ----------------------------- | ----------------------------- |
| user_id  | 用户id，与user_info表中的相同 | 整数型数值，如1，2，3         |
| username | 用户的昵称                    | 字符串，如张三，应当唯一      |
| avatar   | 用户头像文件的文件名          | SHA1(文件名称+上传时间戳).jpg |
| email    | 用户当前的邮箱                | xxx@xxx.com，应当唯一         |

### 用户认证表(user_auth)

用户认证表存储着用户的认证信息，包括密码的数据摘要，token，邮箱验证码等

```mysql
create table user_auth(
	user_id int unsigned not null,
    user_auth tinytext not null,
    auth_salt tinytext not null,
    user_token tinytext not null,
    token_date int not null,
    email_code int not null,
    code_date int not null
)ENGINE=InnoDB DEFAULT CHARSET=utf8;
```

| 字段名     | 说明                                 | 数据格式              |
| ---------- | ------------------------------------ | --------------------- |
| user_id    | 用户id，与user_info表中的相同        | 整数型数值，如1，2，3 |
| user_auth  | 用户的密码，需以哈希函数进行处理     | 数据摘要              |
| auth_salt  | 生成当前用户密码的数据摘要时使用的盐 | 字符串                |
| user_token | 用户token                            | 字符串                |
| token_date | 用户token生成时的unix时间戳          | 整数型数值，精确到秒  |

### 用户邮箱认证码表(user_email_code)

邮箱认证码表存储着邮箱-认证码，用于进行邮箱认证

```mysql
create table user_email_code(
    email tinytext not null,
    email_code int not null,
    code_date int not null,
)ENGINE=InnoDB DEFAULT CHARSET=utf8;
```

| 字段名     | 说明                   | 数据格式             |
| ---------- | ---------------------- | -------------------- |
| email      | 邮箱                   | 字符串               |
| email_code | 对应的验证码           | 6位整数              |
| code_date  | 验证码产生的unix时间戳 | 整数型数值，精确到秒 |

### 用户注册模块

用户注册时应当提供邮箱，用户名，邮箱验证码，密码

具体调用见api文档

### 用户登录模块

用户登录时应提供邮箱或用户名，以及密码

具体调用见api文档

