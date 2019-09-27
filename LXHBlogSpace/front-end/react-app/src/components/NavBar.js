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

import MainContainer from './MainContainer'
import "./css/NavBar.css"


//以下为重构的代码
class NavBar extends React.Component
{
    constructor(props)
    {
        super(props);
        this.state = {
            srcStatusZone: <></>,
            currentTab: "main",
            loginDialogOpenState: false,
            noticeOpenState: false,
            noticeMsg: ""
        }
    }

    render()
    {
        return(
            <AppBar position="fixed" style={{minWidth: 960, background: "#f5f5f5"}}>
                <ToolBar >
                    <Typography className ="navstyle-title" variant="h5">
                        LXH's Blog Space
                    </Typography>
                    <div className="navstyle-index">
                        {this.IndexZone()}
                    </div>
                    <div className="navstyle-status">
                        <>
                            <Button className= "navstyle-button" onClick={this.ShowDialog.bind(this)}>
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
                                        />
                                    </div>
                                </DialogContent>
                                <DialogAction>
                                    <Button onClick={props=>this.DoLoginAction(props)}>登录</Button>
                                    <Button onClick={this.HideDialog.bind(this)}>关闭</Button>
                                </DialogAction>
                                <Snackbar  open={this.state.noticeOpenState} onClose={this.setNoticeClose.bind(this)} message={this.state.noticeMsg} autoHideDuration={3000} />
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
        console.log(this.state);
    }
    
    HideDialog()
    {
        this.setState({
            loginDialogOpenState: false
        })
        
    }

    IndexZone = () => {
        function handleChange(event, newTab) {
            this.setState({
                currentTab: newTab
            });

            ReactDOM.render(MainContainer("container-loading"), document.getElementById('main-panel-root'));
            ReactDOM.render(MainContainer(newTab), document.getElementById('main-panel-root'));
        };
    
        return(
            <Tabs value = {this.state.currentTab} onChange={handleChange.bind(this)} indicatorColor="primary" >
                <Tab className="indexstyle-tabmod" label="首页"  value="main"/>
                <Tab className="indexstyle-tabmod" label="时间线"  value="timeline" />
                <Tab disabled={true} className="indexstyle-tabmod" label="深水池"  value="water"/>
                
            </Tabs>
        );
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
                setTimeout(()=>{window.location="/"}, 3000);
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
        fetch("/api/v1/passport/GetUserInfo", {credentials: "include"})
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
                                <Link href={"/user/" + userinfo.username} underline="none" >{userinfo.username}</Link>
                            </div>
                            <Avatar src={userinfo.avatar}/>
                        </div>
                    </>
                    , document.getElementsByClassName("navstyle-status")[0]);
                        
                }
            });
    }
};

export default NavBar;