## content模块的库表设计

2019-9-30

ver: 1.0

2019-10-4

ver: 1.1

### 数据库

**lxhblogspace_content**

### 图片上传历史表

用于记录哪个用户于何时上传了哪张图片

```mysql
create table image_path(
    uuid tinytext not null,
    create_time int unsigned not null,
    file_path text not null
)ENGINE=InnoDB DEFAULT CHARSET=utf8;
```

| 键名        | 说明                 |
| ----------- | -------------------- |
| uuid        | 用户的UUID           |
| create_time | 文件上传的unix时间戳 |
| file_path   | 文件存储的路径       |

### 草稿信息表

用于保存用户的草稿信息

```mysql
create table draft_info(
    draft_id int unsigned not null primary key auto_increment,
    user_uuid tinytext not null,
    create_date int unsigned not null,
    lastmodify_date int unsigned not null,
    background_img text not null,
    saved_images text not null,
    deleted tinyint unsigned
)ENGINE=InnoDB DEFAULT CHARSET=utf8;
```

| 键名            | 说明                                                         |
| --------------- | ------------------------------------------------------------ |
| draft_id        | 草稿ID，为自增主键                                           |
| user_uuid       | 用户的UUID                                                   |
| create_date     | 草稿创建的unix时间戳                                         |
| lastmodify_date | 上次编辑的unix时间戳                                         |
| background_img  | 背景图片的存储路径                                           |
| saved_images    | 上传的图片，一个js array，请用json.stringfiy()进行序列化后再存储 |
| deleted         | 1为被删除，0为未被删除                                       |

### 草稿内容表

用于保存用户的草稿

```mysql
create table draft_content(
    draft_id int unsigned not null,
    title tinytext not null,
    content mediumtext not null
)ENGINE=InnoDB DEFAULT CHARSET=utf8;
```

| 键名     | 说明                         |
| -------- | ---------------------------- |
| draft_id | 草稿的ID，与draft_info的相同 |
| title    | 文章标题                     |
| content  | 文章内容                     |

### 文章信息表

用于保存用户的文章信息

```mysql
create table article_info(
    article_id int unsigned not null primary key auto_increment,
    user_uuid tinytext not null,
    create_date int unsigned not null,
    background_img text not null,
    vote_num int unsigned not null,
    lastmodify_date int unsigned not null,
    deleted tinyint unsigned
)ENGINE=InnoDB DEFAULT CHARSET=utf8;
```

| 键名            | 说明                     |
| --------------- | ------------------------ |
| article_id      | 文章ID，为自增主键       |
| user_uuid       | 用户的UUID               |
| create_date     | 文章创建的unix时间戳     |
| background_img  | 文章的背景图片的存储路径 |
| vote_num        | 文章的赞数               |
| lastmodify_date | 上次编辑的unix时间戳     |
| deleted         | 1为被删除，0为未被删除   |

### 文章点赞记录表

```mysql
create table article_vote_history(
    article_id int unsigned not null,
    user_uuid tinytext not null,
    action_date int unsigned not null
)ENGINE=InnoDB DEFAULT CHARSET=utf8;
```

| 字段名      | 说明     |
| ----------- | -------- |
| article_id  | 文章id   |
| user_uuid   | 用户uuid |
| action_date | 操作时间 |

### 文章收藏记录表

```mysql
create table article_subscribe_history(
    article_id int unsigned not null,
    user_uuid tinytext not null,
    action_date int unsigned not null
)ENGINE=InnoDB DEFAULT CHARSET=utf8;
```

| 字段名      | 说明     |
| ----------- | -------- |
| article_id  | 文章id   |
| user_uuid   | 用户uuid |
| action_date | 操作时间 |


### 文章内容表

用于保存用户的文章

```mysql
create table article_content(
    article_id int unsigned not null,
    title tinytext not null,
    content mediumtext not null
)ENGINE=InnoDB DEFAULT CHARSET=utf8;
```

| 键名       | 说明                           |
| ---------- | ------------------------------ |
| article_id | 文章的ID，与article_info的相同 |
| title      | 文章标题                       |
| content    | 文章内容                       |

### 文章评论表

```mysql
create table article_comment(
    comment_id int unsigned not null primary key auto_increment,
    article_id int unsigned not null,
    from_uuid tinytext not null,
    to_uuid tinytext not null,
    create_date int unsigned not null,
    content text not null
)ENGINE=InnoDB DEFAULT CHARSET=utf8;
```

| 键名        | 说明                             |
| ----------- | -------------------------------- |
| comment_id  | 评论ID，自增主键                 |
| article_id  | 文章的ID，与article_info的相同   |
| from_uuid   | 发送评论的用户                   |
| to_uuid     | 接受评论的用户，如果没有则置空值 |
| create_date | 创建时间                         |
| content     | 评论内容                         |



### 热点文章表

```mysql
create table article_comment(
    article_id int unsigned not null,
    title tinytext not null,
    heat int unsigned not null
)ENGINE=InnoDB DEFAULT CHARSET=utf8;
```

| 字段名     | 说明                   |
| ---------- | ---------------------- |
| article_id | 文章id                 |
| user_uuid  | 作者的uuid             |
| last_heat  | 上次比较时产生的热度值 |



