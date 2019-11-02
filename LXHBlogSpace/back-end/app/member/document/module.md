## member模块的库表设计



### 用户关注列表

```mysql
create table user_follow(
    user_uuid tinytext not null,
    follower_uuid tinytext not null,
)ENGINE=InnoDB DEFAULT CHARSET=utf8;
```

| 字段名        | 说明         |
| ------------- | ------------ |
| user_uuid     | 用户的UUID   |
| follower_uuid | 关注者的UUID |

### 用户关注信息表

```mysql
create table user_follow_info(
    user_uuid tinytext not null,
    fans_num int unsigned not null,
    followed_num int unsigned not null
)ENGINE=InnoDB DEFAULT CHARSET=utf8;
```

| 字段名       | 说明                     |
| ------------ | ------------------------ |
| user_uuid    | 用户的uuid               |
| fans_num     | 关注该用户的用户数量     |
| followed_num | 该用户关注其他用户的数量 |



### 用户评论消息提示表

```mysql
create table user_comment_notice(
    notice_id int unsigned primary key auto_increment,
    recipient_uuid tinytext not null,
    comment_id int unsigned not null,
    readed tinyint unsigned not null
)ENGINE=InnoDB DEFAULT CHARSET=utf8;
```

| 字段名         | 说明             |
| -------------- | ---------------- |
| notice_id      | 消息ID           |
| recipient_uuid | 消息接收者的uuid |
| comment_id     | 评论的ID         |
| readed         | 该消息是否已读   |



### 系统消息提示表

```mysql
create table sys_notice(
    notice_id int unsigned primary key auto_increment,
    recipient_uuid tinytext not null,
    msg_title tinytext not null,
    msg text not null,
    create_date int unsigned not null,
    readed tinyint unsigned not null
)ENGINE=InnoDB DEFAULT CHARSET=utf8;
```

| 字段名         | 说明             |
| -------------- | ---------------- |
| notice_id      | 消息id           |
| recipient_uuid | 接收者的UUID     |
| msg            | 提示消息         |
| msg_title      | 消息的标题       |
| create_date    | 创建的Unix时间戳 |
| readed         | 是否已经读取     |



### 用户浏览历史表

```mysql
create table user_watch_history(
    user_uuid tinytext not null,
    article_id int unsigned not null,
    watch_date int unsigned not null
)ENGINE=InnoDB DEFAULT CHARSET=utf8;
```

| 字段名     | 说明                           |
| ---------- | ------------------------------ |
| user_uuid  | 用户的uuid                     |
| article_id | 用户浏览的文章                 |
| watch_date | 浏览时间，unix时间戳，精确到秒 |

