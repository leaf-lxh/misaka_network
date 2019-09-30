## content模块的库表设计

2019-9-30

ver: 1.0

### 文件上传历史表

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

