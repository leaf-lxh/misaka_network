import React from 'react';

import AppBar from '@material-ui/core/AppBar'
import ToolBar from '@material-ui/core/Toolbar'
import Typography from '@material-ui/core/Typography'
import Button from '@material-ui/core/Button'
import Tabs from '@material-ui/core/Tabs'
import Tab from '@material-ui/core/Tab'
/*
import Link from '@material-ui/core/Link'
import Avatar from '@material-ui/core/Avatar';
*/
import Dialog from "@material-ui/core/Dialog"
import DialogTitle from "@material-ui/core/DialogTitle"
import DialogAction from "@material-ui/core/DialogActions"
import DialogContent from "@material-ui/core/DialogContent"

import TextField from "@material-ui/core/TextField"

import Snackbar from '@material-ui/core/Snackbar'


import AccountBoxIcon from '@material-ui/icons/AccountBox'
//import AssignmentIcon from '@material-ui/icons/Assignment'

import "./css/Navbar.css"

var API_PROVIDER_SERVER = "http://api.leaflxh.com:4564";


//以下为重构的代码
class NavBar extends React.Component
{
    constructor(props)
    {
        super(props);
        this.state = {
            srcStatusZone: <></>,
            currentTab: "register",
            loginDialogOpenState: false,
            noticeOpenState: false,
            noticeMsg: ""
        }
    }

    render()
    {
        return(
            <AppBar position="fixed" style={{minWidth: 960, background: "#f5f5f5", zIndex: 1201}}>
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


    DoLoginAction(props)
    {
        console.log(props);
        var request = new XMLHttpRequest();
        request.onreadystatechange = function() {
            if (request.status === 302 && request.readyState === 4)
            {
                this.setState({
                    noticeOpenState: true,
                    noticeMsg: "登录成功"
                })

                setTimeout(window.location.reload, 1000);
            }
            else if (request.status === 200 && request.readyState === 4)
            {
                var response = JSON.parse(request.responseText);
                this.setState({
                    noticeOpenState: true,
                    noticeMsg: "登录失败：" + response.reason
                })
            }
        }
        request.open("POST", API_PROVIDER_SERVER + "/api/v1/passport/login", true)
        request.send(null);
    }

    IndexZone = () => {
        function handleChange(event, newTab) {
            if (newTab === "main")
            {
                window.location = "/";
            }
        };
    
        return(
            <Tabs value = {this.state.currentTab} onChange={handleChange.bind(this)} indicatorColor="primary" >
                <Tab className="indexstyle-tabmod" label="首页"  value="main"/>
                <Tab className="indexstyle-tabmod" label="用户注册"  value="register" />
                
            </Tabs>
        );
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
        fetch(API_PROVIDER_SERVER + "/blog/v1/passport/GetUserInfo")
            .then(response=>response.json(), (error) => {

            })
            .then((userinfo)=>{
                console.log(userinfo);
                if (userinfo !== undefined && userinfo.vaild === "true")
                {
                    window.location = "/";                        
                }
            });
    }
};

export default NavBar;