## member模块的库表设计



### 用户关注列表

```mysql
create table user_subscribe(
    user_uuid tinytext not null,
    victim tinytext not null
)ENGINE=InnoDB DEFAULT CHARSET=utf8;
```

| 字段名    | 说明             |
| --------- | ---------------- |
| user_uuid | 用户的uuid       |
| victim    | 被关注用户的uuid |

### 用户评论消息提示表

```mysql
create table user_notice(
    notice_id int unsigned primary key auto_increment,
    from_uuid tinytext not null,
    to_uuid tinytext not null,
    comment_id text not null,
    readed tinyint unsigned not null
)ENGINE=InnoDB DEFAULT CHARSET=utf8;
```

| 字段名     | 说明             |
| ---------- | ---------------- |
| notice_id  | 消息ID           |
| from_uuid  | 消息产生者的uuid |
| to_uuid    | 消息接收者的uuid |
| comment_id | 评论的ID         |
| readed     | 该消息是否已读   |



### 系统消息提示表

```mysql
create table sys_notice(
    notice_id int unsigned primary key auto_increment,
    recipient_uuid tinytext not null,
    msg text not null,
    readed tinyint unsigned not null
)ENGINE=InnoDB DEFAULT CHARSET=utf8;
```

| 字段名         | 说明         |
| -------------- | ------------ |
| notice_id      | 消息id       |
| recipient_uuid | 接收者的uuid |
| msg            | 提示消息     |
| readed         | 是否已经读取 |



### 用户浏览历史表

```mysql
create table user__watch_history(
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

