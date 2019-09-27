import React from 'react';
import ReactDOM from 'react-dom'
import AppBar from '@material-ui/core/AppBar'
import ToolBar from '@material-ui/core/Toolbar'
import Typography from '@material-ui/core/Typography'
//import Button from '@material-ui/core/Button'
import Tabs from '@material-ui/core/Tabs'
import Tab from '@material-ui/core/Tab'
import Link from '@material-ui/core/Link'
import Avatar from '@material-ui/core/Avatar';

/*
import Dialog from "@material-ui/core/Dialog"
import DialogTitle from "@material-ui/core/DialogTitle"
import DialogAction from "@material-ui/core/DialogActions"
import DialogContent from "@material-ui/core/DialogContent"

import TextField from "@material-ui/core/TextField"

import Snackbar from '@material-ui/core/Snackbar'


import AccountBoxIcon from '@material-ui/icons/AccountBox'
import AssignmentIcon from '@material-ui/icons/Assignment'
*/
import "./css/Navbar.css"

//var API_PROVIDER_SERVER = "http://api.leaflxh.com";


//以下为重构的代码
class NavBar extends React.Component
{
    constructor(props)
    {
        super(props);
        this.state = {
            srcStatusZone: <></>,
            currentTab: "write_article",
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
                        
                    </div>
                </ToolBar>
            </AppBar>
        )
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
                <Tab className="indexstyle-tabmod" label="创作"  value="write_article" />
                
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
        fetch("/api/v1/GetUserInfo")
            .then(response=>response.json(), (error) => {
                //window.location = "/";
            })
            .then((userinfo)=>{
                if (userinfo.vaild)
                {
                    ReactDOM.render(
                    <>
                        <div className="navstyle-userZone">
                            <div className="navstyle-userId">
                                <Link href={"/user/" + userinfo.name} underline="none" >{userinfo.name}</Link>
                            </div>
                            <Avatar src={userinfo.avatar}/>
                        </div>
                    </>
                    , document.getElementsByClassName("navstyle-status")[0]);
                        
                }
                else
                {
                    //window.location = "/";
                }
            });
    }
};

export default NavBar;
