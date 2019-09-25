use lxhblogspace_passport;

create table user_info(
    user_uuid tinytext  not null,
    register_date int unsigned not null,
    email tinytext not null,
    username tinytext not null,
    is_locked int not null
)ENGINE=InnoDB DEFAULT CHARSET=utf8;

create table user_details(
	user_uuid tinytext not null,
    avatar tinytext not null,
    description tinytext not null
)ENGINE=InnoDB DEFAULT CHARSET=utf8;

create table user_auth(
	user_uuid tinytext not null,
    auth_hex tinytext not null,
    auth_salt tinytext not null
)ENGINE=InnoDB DEFAULT CHARSET=utf8;

create table user_token(
    user_uuid tinytext not null,
    user_token tinytext not null,
    token_date int not null
)ENGINE=InnoDB DEFAULT CHARSET=utf8;

create table user_email_code(
    email tinytext not null,
    email_code int not null,
    code_date int not null
)ENGINE=InnoDB DEFAULT CHARSET=utf8;