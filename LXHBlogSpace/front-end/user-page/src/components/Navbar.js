import React from 'react';
import ReactDOM from 'react-dom'
import AppBar from '@material-ui/core/AppBar'
import ToolBar from '@material-ui/core/Toolbar'
import Typography from '@material-ui/core/Typography'
import Button from '@material-ui/core/Button'
import Tabs from '@material-ui/core/Tabs'
import Tab from '@material-ui/core/Tab'
import Link from '@material-ui/core/Link'
import Avatar from '@material-ui/core/Avatar';

import Dialog from "@material-ui/core/Dialog"
import DialogTitle from "@material-ui/core/DialogTitle"
import DialogAction from "@material-ui/core/DialogActions"
import DialogContent from "@material-ui/core/DialogContent"

import TextField from "@material-ui/core/TextField"

import Snackbar from '@material-ui/core/Snackbar'


import AccountBoxIcon from '@material-ui/icons/AccountBox'
import AssignmentIcon from '@material-ui/icons/Assignment'

import "./css/NavBar.css"


//以下为重构的代码
class NavBar extends React.Component
{
    constructor(props)
    {
        super(props);
        this.state = {
            srcStatusZone: <></>,
            currentTab: "article",
            loginDialogOpenState: false,
            resetDialogOpenState: false,
            noticeOpenState: false,
            noticeMsg: "",
            setIntervalID: null,
            sendEmailButtonDisabled: false,
            sendEmailButtonContent: "发送验证码"
        }
    }

    KeyDownHandler(props)
    {
        if(props.key === "Enter")
        {
            this.DoLoginAction();
        }
    }
    
    toResetDialog()
    {
        this.HideDialog();
        this.setState({
            resetDialogOpenState:true,
            
        })
    }

    HideResetDialog()
    {
        this.setState({
            resetDialogOpenState:false,
            noticeOpenState: false,
            noticeMsg: ""
        })
    }

    CheckEmailFmt(email)
    {
        if (email === "")
        {
            return false;
        }

        return (email.match(/[0-9a-zA-Z.+-_]+?@[0-9a-zA-Z.]*?(qq.com|163.com|gmail.com|foxmail.com|126.com|sina.com)/) !== null)
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
            this.setState({
                time: 0,
                sendEmailButtonDisabled: false,
                sendEmailButtonContent: "发送验证码"
            })
            clearTimeout(this.state.setIntervalID);
        }
    }


    DoSendEmailCode()
    {
        var email = document.getElementById("reset-login-email").value;
        if (this.CheckEmailFmt(email) === false)
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
                    sendEmailButtonContent: "已发送(60s)",
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

    CheckPassword(password)
    {
        if (password === "")
        {
            return false;
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
                    return false;
                }
            }
    
            if (foundAlpha === false || foundDigit === false)
            {
                return false;
            }
            else
            {
                return true;
            }
        }
        else
        {
            return false;
        }
    }

    DoResetPassword()
    {
        var email = document.getElementById("reset-login-email").value;
        var password = document.getElementById("reset-login-password").value;
        var vcode = document.getElementById("reset-email-code").value;

        if(this.CheckPassword(password) === false)
        {
            this.setState({
                noticeMsg: "密码需要满足8-15个字符的长度，并包含字母和数字。可添加下划线",
                noticeOpenState: true
            })
            return ;
        }

        fetch("/api/v1/passport/UpdatePassword", {
            'method': 'POST',
            'headers':{
                'Content-Type': "application/x-www-form/urlencoded"
            },
            'body': "email=" + email +"&vcode="+vcode + "&password=" + password
        })
        .then(res=>{
            if (res.ok !== true)
            {
                this.setState({
                    noticeOpenState: true,
                    noticeMsg: "服务器未响应，请稍后再试"
                })
                return;
            }
            else
            {
                return res.json()
            }
        })
        .then(res=>{
            if (res !== undefined && res.ecode === 0)
            {
                this.setState({
                    noticeOpenState: true,
                    noticeMsg: "更新密码成功，即将跳转至登录框"
                })
                setTimeout(function(){this.setState({resetDialogOpenState:false, loginDialogOpenState:true})}.bind(this), 2000);
            }
            else
            {
                this.setState({
                    noticeOpenState: true,
                    noticeMsg: "修改密码失败，原因：" + res.reason
                })
            }
        })
    }

    render()
    {
        document.addEventListener("nav-loaded", function(){ReactDOM.render(this.container, document.getElementById('main-panel-root'))}.bind(this))
        return(
            <AppBar position="fixed" style={{minWidth: 960, background: "#f5f5f5", zIndex: 1400}}>
                <ToolBar >
                    <Link className ="navstyle-title" variant="h5" href="/" underline="none">
                        LXH's Blog Space
                    </Link>
                    <div className="navstyle-status">
                        <>
                            <Button className= "navstyle-button" id="open-login-button" onClick={this.ShowDialog.bind(this)}>
                                登录
                                <AccountBoxIcon className="navstyle-icon" />
                            </Button>
                            <Button className="navstyle-button" href="/join">
                                注册
                                <AssignmentIcon className="navstyle-icon" />
                            </Button>
                            <Dialog open={this.state.loginDialogOpenState} onClose={this.HideDialog.bind(this)}>
                                <DialogTitle>登录</DialogTitle>
                                <DialogContent >
                                    <div>
                                        <TextField
                                            id="login-username"
                                            label="用户名或邮箱"
                                            type="email"
                                            name="email"
                                            autoComplete="email"
                                            margin="normal"
                                            style={{margin: "0 auto 0 auto", minWidth: "300px"}}
                                        />
                                    </div>
                                    <div>
                                        <TextField
                                            id="login-password"
                                            label="密码"
                                            type="password"
                                            name="password"
                                            autoComplete="password"
                                            margin="normal"
                                            style={{marginTop: "15px", minWidth: "300px"}}
                                            onKeyUp={this.KeyDownHandler.bind(this)}
                                        />
                                    </div>
                                </DialogContent>
                                <DialogAction>
                                    <Button onClick={props=>this.DoLoginAction(props)}>登录</Button>
                                    <Button onClick={this.toResetDialog.bind(this)}>忘记密码</Button>
                                </DialogAction>
                                <Snackbar style={{zIndex: 1301}}  open={this.state.noticeOpenState} onClose={this.setNoticeClose.bind(this)} message={this.state.noticeMsg} autoHideDuration={3000} />
                            </Dialog>
                            <Dialog open={this.state.resetDialogOpenState} onClose={this.HideResetDialog.bind(this)}>
                                <DialogTitle>重置密码</DialogTitle>
                                <DialogContent >
                                    <div>
                                        <TextField
                                            id="reset-login-email"
                                            label="邮箱"
                                            type="email"
                                            name="email"
                                            autoComplete="email"
                                            margin="normal"
                                            style={{margin: "0 auto 0 auto", minWidth: "300px"}}
                                        />
                                    </div>
                                    <div>
                                        <TextField
                                            id="reset-login-password"
                                            label="密码"
                                            type="password"
                                            name="password"
                                            autoComplete="password"
                                            margin="normal"
                                            style={{marginTop: "15px", minWidth: "300px"}}
                                        />
                                    </div>
                                    <div>
                                        <TextField
                                            id="reset-email-code"
                                            label="验证码"
                                            type="text"
                                            name="text"
                                            autoComplete="text"
                                            margin="normal"
                                            style={{marginTop: "15px", minWidth: "300px"}}
                                        />
                                    </div>
                                </DialogContent>
                                <DialogAction>
                                    <Button onClick={this.DoSendEmailCode.bind(this)} disabled={this.state.sendEmailButtonDisabled}>{this.state.sendEmailButtonContent}</Button>
                                    <Button onClick={this.DoResetPassword.bind(this)}>重置</Button>
                                </DialogAction>
                                <Snackbar style={{zIndex: 1301}}  open={this.state.noticeOpenState} onClose={this.setNoticeClose.bind(this)} message={this.state.noticeMsg} autoHideDuration={3000} />
                            </Dialog>
                        </>
                    </div>
                </ToolBar>
            </AppBar>
        )
    }

    ShowDialog()
    {
        this.setState({
            loginDialogOpenState: true
        })
    }
    
    HideDialog()
    {
        this.setState({
            loginDialogOpenState: false,
            noticeOpenState: false,
            noticeMsg: ""
        })
        
    }

    DoLoginAction()
    {
        var username = document.getElementById("login-username").value;
        var password = document.getElementById("login-password").value;

        fetch("/api/v1/passport/login", {
            method: "POST",
            header: {
                "Content-Type": "application/x-www-form-urlencoded",
            },
            body: encodeURI("username=" + username + "&password=" + password)
        }).then(res=>{
            if (res.status === 302)
            {
                this.setState({
                    noticeOpenState: true,
                    noticeMsg: "登录成功"
                })
                setTimeout(()=>{window.location.reload()}, 1500);
                return;
            }
            else if (res.status === 200)
            {
               return res.json()
            }
            else
            {

                this.setState({
                    noticeOpenState: true,
                    noticeMsg: "登录失败，服务器未响应"
                })
                return;
            }
        }, (error)=>{
            console.log(error);
        })
        .then(json=>{
            if (json !== undefined)
            {
                this.setState({
                    noticeOpenState: true,
                    noticeMsg: "登录失败，原因：" + json.reason
                })
            }
        })
    }

    setNoticeClose()
    {
        this.setState({
            noticeOpenState: false
        })
    }


    componentDidMount()
    {
        //初始化右边的状态栏，流程为：检查是否登录，如果是则显示用户信息，否则显示登录按钮
        fetch("/api/v1/passport/IsLogin", {credentials: "include"})
            .then(response=>response.json(), (error) => {
                this.setState({
                    srcStatusZone: this.LoginRegion()
                });
            })
            .then((userinfo)=>{
                if (userinfo.vaild === "true")
                {
                    ReactDOM.render(
                    <>
                        <div className="navstyle-userZone">
                            <div className="navstyle-userId">
                                <Link href={"/member/" + userinfo.username} underline="none" >{userinfo.username}</Link>
                            </div>
                            <Avatar src={userinfo.avatar}/>
                        </div>
                    </>
                    , document.getElementsByClassName("navstyle-status")[0]);
                    window.user_info = {"username": userinfo.username, "avatar": userinfo.avatar};
                    window.logined_username = userinfo.username;
                }
                var event = new Event("nav-loaded")  ;
                document.dispatchEvent(event)
            });
    }
};

export default NavBar;