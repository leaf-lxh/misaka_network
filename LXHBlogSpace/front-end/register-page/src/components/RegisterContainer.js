import React from "react"
import Container from "@material-ui/core/Container"
import "./css/RegisterContainer.css"

import Button from "@material-ui/core/Button"
import Input from "@material-ui/core/Input"
import InputLabel from "@material-ui/core/InputLabel"

import API_SERVICE_DOMAIN from "./APIService"

class RegisterContainer extends React.Component{
    constructor(props)
    {
        super(props);
        this.state = {
            notice_username: "小于10个字符, 并且唯一",
            notice_username_style: "register-inputnotice",

            notice_password: "长度为8-15位，需包含字母和数字，可添加下划线",
            notice_notice_password_style: "register-inputnotice",

            notice_email: "仅支持腾讯，网易，新浪，谷歌邮箱",
            notice_email_style: "register-inputnotice",

            notice_emailcode: "",
            notice_emailcode_style: "register-inputnotice"
        };
    }

    CheckUserName()
    {
        var username_input = document.getElementById("input-username");
        var username = username_input.value;
        if (username === "")
        {
            this.setState({
                notice_username: "小于10个字符, 并且唯一",
                notice_username_style: "register-inputnotice"
            })
            return;
        }

        if (username.length <= 10)
        {
            fetch(API_SERVICE_DOMAIN + "/blog/v1/passport/CheckUserExist").then(res=>res.json())
            .then(json=>{
                if (json !== undefined && json.exist === "true")
                {
                    this.setState({
                        notice_username: "此用户名已存在",
                        notice_username_style: "register-inputnotice-err"
                    });
                }
                else
                {
                    this.setState({
                        notice_username: "用户名符合要求",
                        notice_username_style: "register-inputnotice-ok"
                    });
                }
            })

        }
        else
        {
            this.setState({
                notice_username: "用户名过长，当前长度为：" + username.length,
                notice_username_style: "register-inputnotice-err"
            });
        }
    }

    render()
    {
        return (
            <Container fixed className="register-container">
                <InputLabel className="register-inputlabel">用户名</InputLabel>
                <div className="input-region">
                    <Input id="input-username" className="register-input" disableUnderline classes={{focused: 'focused'}} onBlur={this.CheckUserName.bind(this)}></Input>
                    <InputLabel className={this.state.notice_username_style} variant="filled">{this.state.notice_username}</InputLabel>
                </div>

                <InputLabel className="register-inputlabel">密码</InputLabel>
                <div className="input-region">
                    <Input id="input-password" className="register-input" disableUnderline classes={{focused: 'focused'}} type="password"></Input>
                    <InputLabel className={this.state.notice_notice_password_style} variant="filled">{this.state.notice_password}</InputLabel>
                </div>

                <InputLabel className="register-inputlabel">邮箱</InputLabel>
                <div className="input-region">
                    <Input id="input-email" className="register-input" disableUnderline classes={{focused: 'focused'}}></Input>
                    <InputLabel className={this.state.notice_email_style} variant="filled">{this.state.notice_email}</InputLabel>
                </div>

                <InputLabel className="register-inputlabel">邮箱验证码</InputLabel>
                <div className="input-region">
                    <Input id="input-emailcode" className="register-input" disableUnderline classes={{focused: 'focused'}}></Input>
                    <Button className="register-emailcode-button">获取验证码</Button>
                </div>
                <div className="register-submit">
                    <Button className="register-submit-button">注册</Button>
                </div>
                
            </Container>
        )
    }
}

export default RegisterContainer;