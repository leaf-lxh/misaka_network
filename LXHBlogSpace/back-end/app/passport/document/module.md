## passport

2019-07-09

ver 1.0

### 数据库

**lxhblogspace_passport**

### 用户信息表(user_info)

用户信息表为后台信息数据的主体之一，用于登录的流程

```mysql
create table user_info(
    user_uuid tinytext  not null,
    register_date int unsigned not null,
    email tinytext not null,
    username tinytext not null,
    is_locked int not null
)ENGINE=InnoDB DEFAULT CHARSET=utf8;
```

| 字段名        | 说明                           | 数据格式                   |
| ------------- | ------------------------------ | -------------------------- |
| user_uuid | 用户UUID                 | 字符值UUID                 |
| register_date | 用户注册时间                   | 整数型unix时间戳，精确到秒 |
| email    | 用户当前的邮箱                | xxx@xxx.com，应当唯一         |
| username | 用户的昵称                    | 字符串，如张三，应当唯一      |
| is_locked     | 是否被锁定，1为锁定，0为未锁定 | 整数型数值                 |

### 用户详细信息表(user_details)

用户详细信息表存储着用户的个人资料。为了减轻数据库的负担，数据约束上在应用层上做

```mysql
create table user_details(
	user_uuid tinytext not null,
    avatar tinytext not null,
    description tinytext not null
)ENGINE=InnoDB DEFAULT CHARSET=utf8;
```

| 字段名      | 说明                            | 数据格式                      |
| ----------- | ------------------------------- | ----------------------------- |
| user_uuid   | 用户UUID，与user_info表中的相同 | 字符值UUID                    |
| avatar      | 用户头像文件的文件名            | SHA1(文件名称+上传时间戳).jpg |
| description | 用户简介，小于等于200个字符     | 字符串                        |


### 用户认证表(user_auth)

用户认证表存储着用户的认证信息，包括密码的数据摘要，token，邮箱验证码等

```mysql
create table user_auth(
	user_uuid tinytext not null,
    auth_hex tinytext not null,
    auth_salt tinytext not null
)ENGINE=InnoDB DEFAULT CHARSET=utf8;
```

| 字段名    | 说明                                                       | 数据格式       |
| --------- | ---------------------------------------------------------- | -------------- |
| user_uuid | 用户UUID，与user_info表中的相同                            | 字符值UUID     |
| auth_hex  | SHA1(MD5(用户密码)+auth_salt)                              | 字符串数据摘要 |
| auth_salt | 生成当前用户密码的数据摘要时使用的盐，使用UUID确保不可预测 | 字符串         |

### 用户令牌表(user_token)

用户令牌表，用于保存用户登录凭证。一个用户可存放多个令牌。每个令牌的有效期为30天

```mysql
create table user_token(
    user_uuid tinytext not null,
    user_token tinytext not null,
    token_date int not null
)ENGINE=InnoDB DEFAULT CHARSET=utf8;
```

| 字段名 | 说明 | 数据格式 |
| ---- | ---- | ---- |
| user_uuid  | 用户UUID，与user_info表中的相同                            | 字符值UUID           |
| token_value | SHA1(auth_hex + 随机UUID) + 14个A + SHA1(前面的内容)       | 字符串               |
| token_date | 用户token生成时的unix时间戳                                | 整数型数值，精确到秒 |



### 用户邮箱认证码表(user_email_code)

邮箱认证码表存储着邮箱-认证码，用于进行邮箱认证

```mysql
create table user_email_code(
    email tinytext not null,
    email_code int not null,
    code_date int not null
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

具体流程：

通过用户名或邮箱查找该用户的UUID

通过此UUID查找用户的密码

计算此密码是否匹配

如果匹配，则设置用户令牌，并SetCookie，键名为`_sessionToken`

### 用户登录模块

用户登录时应提供邮箱或用户名，以及密码

具体调用见api文档

