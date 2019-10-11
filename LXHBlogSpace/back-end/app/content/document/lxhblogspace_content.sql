drop database lxhblogspace_content;
create database lxhblogspace_content;
use lxhblogspace_content;

create table image_path(
    uuid tinytext not null,
    create_time int unsigned not null,
    file_path text not null
)ENGINE=InnoDB DEFAULT CHARSET=utf8;

create table draft_info(
    draft_id int unsigned not null primary key auto_increment,
    user_uuid tinytext not null,
    create_date int unsigned not null,
    lastmodify_date int unsigned not null,
    background_img text not null,
    saved_images text not null,
    deleted tinyint unsigned
)ENGINE=InnoDB DEFAULT CHARSET=utf8;

create table draft_content(
    draft_id int unsigned not null,
    title tinytext not null,
    content mediumtext not null
)ENGINE=InnoDB DEFAULT CHARSET=utf8;

create table article_info(
    article_id int unsigned not null primary key auto_increment,
    user_uuid tinytext not null,
    create_date int unsigned not null,
    background_img text not null,
    vote_num int unsigned not null,
    lastmodify_date int unsigned not null,
    deleted tinyint unsigned
)ENGINE=InnoDB DEFAULT CHARSET=utf8;

create table article_vote_history(
    article_id int unsigned not null,
    user_uuid tinytext not null,
    action_date int unsigned not null
)ENGINE=InnoDB DEFAULT CHARSET=utf8;

create table article_subscribe_history(
    article_id int unsigned not null,
    user_uuid tinytext not null,
    action_date int unsigned not null
)ENGINE=InnoDB DEFAULT CHARSET=utf8;

create table article_content(
    article_id int unsigned not null,
    title tinytext not null,
    content mediumtext not null
)ENGINE=InnoDB DEFAULT CHARSET=utf8;


create table article_comment(
    comment_id int unsigned not null primary key auto_increment,
    article_id int unsigned not null,
    from_uuid tinytext not null,
    to_uuid tinytext not null,
    create_date int unsigned not null,
    content text not null
)ENGINE=InnoDB DEFAULT CHARSET=utf8;