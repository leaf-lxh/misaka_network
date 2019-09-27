import React from "react"
import Container from "@material-ui/core/Container"
import "./css/RegisterContainer.css"

import Button from "@material-ui/core/Button"
import Input from "@material-ui/core/Input"
import InputLabel from "@material-ui/core/InputLabel"
import Snackbar from "@material-ui/core/Snackbar"
import Slide from "@material-ui/core/Slide"


class RegisterContainer extends React.Component{
    constructor(props)
    {
        super(props);
        this.state = {
            notice_username: "小于10个字符, 并且唯一",
            notice_username_style: "register-inputnotice",

            notice_password: "长度为8-15位，需包含字母和数字，可添加下划线",
            notice_password_style: "register-inputnotice",

            notice_email: "仅支持腾讯，网易，新浪，谷歌邮箱",
            notice_email_style: "register-inputnotice",

            notice_emailcode: "",
            notice_emailcode_style: "register-inputnotice",

            ok_username: false,
            ok_password: false,
            ok_email: false,

            noticeOpenState: false,
            noticeMsg: "",

            time: 60,
            sendEmailButtonContent: "发送认证码",
            sendEmailButtonDisabled: false,
            setIntervalID: undefined
        };
    }

    setNoticeClose()
    {
        this.setState({
            noticeOpenState: false
        })
    }

    CheckUserName()
    {
        var username_input = document.getElementById("input-username");
        var username = username_input.value;
        if (username === "")
        {
            this.setState({
                notice_username: "小于10个字符, 并且唯一",
                notice_username_style: "register-inputnotice",
                ok_username: false
            })
            
            return;
        }

        if (username.length <= 10)
        {
            this.setState({
                notice_username: "检查中...",
                notice_username_style: "register-inputnotice-warn",
                ok_username: false
            });

            fetch("/api/v1/passport/CheckUserExist?username=" + username).then(res=>res.json(), (error)=>{})
            .then(json=>{
                if (json === undefined)
                {
                    this.setState({
                        notice_username: "服务器未响应，请稍后再试",
                        notice_username_style: "register-inputnotice-warn",
                        ok_username: false
                    });
                    return;
                }

                if (json.exist === "true")
                {
                    this.setState({
                        notice_username: "此用户名已存在",
                        notice_username_style: "register-inputnotice-err",
                        ok_username: false
                    });
                }
                else
                {
                    this.setState({
                        notice_username: "用户名可以使用",
                        notice_username_style: "register-inputnotice-ok",
                        ok_username: true
                    });
                }
            })

        }
        else
        {
            this.setState({
                notice_username: "用户名过长，当前长度为：" + username.length,
                notice_username_style: "register-inputnotice-err",
                ok_username: false
            });
        }
    }

    CheckPassword()
    {
        var password_input = document.getElementById("input-password");
        var password = password_input.value;

        if (password === "")
        {
            this.setState({
                notice_password: "长度为8-15位，需包含字母和数字，可添加下划线",
                notice_notice_password_style: "register-inputnotice",
                ok_password: false
            })
            return;
        }
        if (password.length >= 8 && password.length <= 15)
        {
            var foundAlpha = false;
            var foundDigit = false;
            for (var i = 0; i < password.length; ++i)
            {
                var chr = password.charAt(i);
                if ((chr >= 'A' && chr <= 'Z') || (chr >= 'a' && chr <= 'z'))
                {
                    foundAlpha = true;
                }
                else if (chr >= '0' && chr <= '9')
                {
                    foundDigit = true;
                }
                else if (chr === '_')
                {
                    continue;
                }
                else
                {
                    this.setState({
                        notice_password: "密码中只能由字母、数字，和下划线组成",
                        notice_password_style: "register-inputnotice-err",
                        ok_password: false
                    });
                    return;
                }
            }
    
            if (foundAlpha === false || foundDigit === false)
            {
                this.setState({
                    notice_password: "当前密码中缺少字母或数字",
                    notice_password_style: "register-inputnotice-err",
                    ok_password: false
                });
            }
            else
            {
                this.setState({
                    notice_password: "当前密码可以使用",
                    notice_password_style: "register-inputnotice-ok",
                    ok_password: true
                });
            }
        }
        else
        {
            this.setState({
                notice_password: "密码长度不符合要求，当前长度：" + password.length,
                notice_password_style: "register-inputnotice-err",
                ok_password: false
            });
        }
    }

    CheckEmail()
    {
        var email_input = document.getElementById("input-email");
        var email = email_input.value;
        if (email === "")
        {
            this.setState({
                notice_email: "仅支持腾讯，网易，新浪，谷歌邮箱",
                notice_email_style: "register-inputnotice"
            });
            return;
        }

        if (email.match(/[0-9a-zA-Z.+-_]+?@[0-9a-zA-Z.]*?(qq.com|163.com|gmail.com|foxmail.com|126.com|sina.com)/) !== null)
        {
            this.setState({
                notice_email: "检查中...",
                notice_email_style: "register-inputnotice-warn"
            });

            fetch("/api/v1/passport/CheckEmailExist?email=" + email).then(res=>res.json(), (error)=>{})
            .then(response=>{
                if (response === undefined)
                {
                    this.setState({
                        notice_email: "服务器未响应，请稍后再试",
                        notice_email_style: "register-inputnotice-warn"
                    });
                    return;
                }

                if (response.exist === "false")
                {
                    this.setState({
                        notice_email: "当前邮箱可以使用",
                        notice_email_style: "register-inputnotice-ok",
                        ok_email: true
                    });
                }
                else if (response.exist === "true")
                {
                    this.setState({
                        notice_email: "当前邮箱已被注册",
                        notice_email_style: "register-inputnotice-err"
                    });
                }
            });
        }
        else
        {
            this.setState({
                notice_email: "仅支持腾讯，网易，新浪，谷歌邮箱",
                notice_email_style: "register-inputnotice-err"
            });
        }

    }

    SendEmailTimeoutHook()
    {
        
        if (this.state.time >= 1)
        {
            this.setState({
                time: this.state.time - 1,
                sendEmailButtonDisabled: true,
                sendEmailButtonContent: this.state.time + " 秒"
            })
        }
        else
        {
            console.log(this.state);
            this.setState({
                time: 0,
                sendEmailButtonDisabled: false,
                sendEmailButtonContent: "发送验证码"
            })
            clearTimeout(this.state.setIntervalID);
        }
    }

    SendEmailAuthCode()
    {
        var email_input = document.getElementById("input-email");
        var email = email_input.value;
        console.log(this.state);
        if (this.state.ok_email === false)
        {
            this.setState({
                noticeMsg: "请检查邮箱格式",
                noticeOpenState: true
            })
            return;
        }

        fetch("/api/v1/passport/SendEmailAuth", {
            method: "POST",
            body: encodeURI("email="+email),
            headers : {
                'Content-Type': 'application/x-www-form-urlencoded'
            }
        }).then(res=>res.json())
        .then((response)=>{
            if (response === undefined)
            {
                this.setState({
                    noticeMsg: "服务器未响应，请稍后再试",
                    noticeOpenState: true
                })
                return;
            }

            if (response.ecode === "0")
            {
                this.setState({
                    noticeMsg: "认证码发送成功",
                    noticeOpenState: true,

                    time: 60,
                    sendEmailButtonContent: "60 秒",
                    sendEmailButtonDisabled: true
                })
                var id = setInterval(this.SendEmailTimeoutHook.bind(this), 1000);
                this.setState({
                    setIntervalID: id
                })
            }
            else
            {
                this.setState({
                    noticeMsg: "发送失败，原因：" + response.reason,
                    noticeOpenState: true
                })
            }
        })
        
    }

    TransitionLeft(props) {
        return <Slide {...props} direction="left" />;
    }

    Login(username, password)
    {
        fetch("/api/v1/passport/login",{
            method: "POST",
            headers: {
                'Content-Type': 'application/x-www-form-urlencoded'
            },
            body: encodeURI('username=' + username + "&password=" + password)
        })
    }

    Register()
    {
        var vcode = document.getElementById("input-emailcode").value;
        var email = document.getElementById("input-email").value;
        var password = document.getElementById("input-password").value;
        var username = document.getElementById("input-username").value;

        if (this.state.ok_email && this.state.ok_username && this.state.ok_password && (vcode.length !== 0))
        {
            fetch("/api/v1/passport/register", {
                method: "POST",
                headers: {
                    'Content-Type': 'application/x-www-form-urlencoded'
                },
                body: encodeURI("username=" + username + "&password=" + password + "&email=" + email + "&vcode=" + vcode)
            }).then(res=>res.json(), (error)=>{})
            .then(response=>{
                if (response === undefined)
                {
                    this.setState({
                        noticeMsg: "服务器未响应，请稍后再试",
                        noticeOpenState: true
                    })
                    return;
                }

                if (response.ecode === "0")
                {
                    this.setState({
                        noticeMsg: "注册成功，即将跳转至首页",
                        noticeOpenState: true
                    })
                    this.Login(username, password);
                    setTimeout(()=>{window.location = "/";}, 3000);
                    return;
                }
                else
                {
                    this.setState({
                        noticeMsg: "注册失败，原因：" + response.reason,
                        noticeOpenState: true
                    })
                    return ;
                }
            })

        }
        else
        {
            this.setState({
                noticeMsg: "请完善注册信息",
                noticeOpenState: true
            })
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
                    <Input id="input-password" className="register-input" disableUnderline classes={{focused: 'focused'}} type="password" onBlur={this.CheckPassword.bind(this)}></Input>
                    <InputLabel className={this.state.notice_password_style} variant="filled">{this.state.notice_password}</InputLabel>
                </div>

                <InputLabel className="register-inputlabel">邮箱</InputLabel>
                <div className="input-region">
                    <Input id="input-email" className="register-input" disableUnderline classes={{focused: 'focused'}} onBlur={this.CheckEmail.bind(this)}></Input>
                    <InputLabel className={this.state.notice_email_style} variant="filled">{this.state.notice_email}</InputLabel>
                </div>

                <InputLabel className="register-inputlabel">邮箱验证码</InputLabel>
                <div className="input-region">
                    <Input id="input-emailcode" className="register-input" disableUnderline classes={{focused: 'focused'}}></Input>
                    <Button className="register-emailcode-button" onClick={this.SendEmailAuthCode.bind(this)} disabled={this.state.sendEmailButtonDisabled}>{this.state.sendEmailButtonContent}</Button>
                </div>
                <div className="register-submit">
                    <Button className="register-submit-button" onClick={this.Register.bind(this)}>注册</Button>
                </div>
                <Snackbar anchorOrigin={{vertical:'bottom', horizontal:'right'}} open={this.state.noticeOpenState} onClose={this.setNoticeClose.bind(this)} message={this.state.noticeMsg} autoHideDuration={3000} TransitionComponent={this.TransitionLeft}/>
                
            </Container>
        )
    }
}

export default RegisterContainer;