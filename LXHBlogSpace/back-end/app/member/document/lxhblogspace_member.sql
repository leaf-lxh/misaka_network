drop database if exists lxhblogspace_member;
create database lxhblogspace_member;
use lxhblogspace_member;

create table user_follow(
    user_uuid tinytext not null,
    follower_uuid tinytext not null,
)ENGINE=InnoDB DEFAULT CHARSET=utf8;

create table user_follow_info(
    user_uuid tinytext not null,
    fans_num int unsigned not null,
    followed_num int unsigned not null
)ENGINE=InnoDB DEFAULT CHARSET=utf8;

create table user_comment_notice(
    notice_id int unsigned primary key auto_increment,
    recipient_uuid tinytext not null,
    comment_id int unsigned not null,
    readed tinyint unsigned not null
)ENGINE=InnoDB DEFAULT CHARSET=utf8;

create table sys_notice(
    notice_id int unsigned primary key auto_increment,
    recipient_uuid tinytext not null,
    msg_title tinytext not null,
    msg text not null,
    create_date int unsigned not null,
    readed tinyint unsigned not null
)ENGINE=InnoDB DEFAULT CHARSET=utf8;

create table user_watch_history(
    user_uuid tinytext not null,
    article_id int unsigned not null,
    watch_date int unsigned not null
)ENGINE=InnoDB DEFAULT CHARSET=utf8;