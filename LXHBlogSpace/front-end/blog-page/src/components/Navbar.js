import React from 'react';
import ReactDOM from 'react-dom';
import { makeStyles } from '@material-ui/core/styles'
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

import "./css/Navbar.css"


const indexStyle = makeStyles({
    indicatorColor : {
        background : "black"
    }

});

const statusStyle = makeStyles({
    button : {
        padding : "5px 50px 5px 50px ",
        background : "#e0e0e0",
        marginLeft : "8px",
        "&:hover" : {
            background : "#d2d0d0"
        },
    },
    icon : {
        paddingLeft : "5px",
    },
    userZone : {
        display : "flex",
        minWidth : 220,
        maxWidth : 220
    },
    userId : {
        flexGrow : 1,
        minWidth : 160,
        maxWidth : 160,
        color : "black",
        textAlign : "right",
        margin : "auto",
        paddingRight : 20,
        whiteSpace : "nowrap",
        overflow : "hidden",
        textOverflow : "ellipsis"

    }
});

var noticeOpenState = false;
// eslint-disable-next-line
var NoticeOpenStateModifier = undefined;

var noticeMsg = "";
var noticeMsgModifier = undefined;

// eslint-disable-next-line
var noticeType = "success";
// eslint-disable-next-line
var noticeTypeModifier = null;

const setOpen = () =>{
    NoticeOpenStateModifier(true);
}

const setClose = () =>{
    NoticeOpenStateModifier(false);
}


function DoLoginAction()
{
    var success = true;
    if (success)
    {
        noticeMsgModifier("登录成功");
        noticeTypeModifier("notice-success")
    }
    else
    {
        noticeMsgModifier("登录失败")
        noticeTypeModifier("notice-error")
    }
    setOpen();
    console.log(noticeOpenState);
}

function LoginRegion(hookEvent, className, style)
{
    /* for popup login form */
    function ShowDialog()
    {
        hookEvent[1](true);
    }

    function HideDialog()
    {
        hookEvent[1](false);
    }

    /* return login form */
    return (
    <>
        <Button className={style.button} onClick={ShowDialog}>
            登录
            <AccountBoxIcon className={style.icon} />
        </Button>
        <Button className={style.button} href="/join">
            注册
            <AssignmentIcon className={style.icon} />
        </Button>
        <Dialog open={hookEvent[0]} onClose={HideDialog}>
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
                <Button onClick={DoLoginAction}>登录</Button>
                <Button onClick={HideDialog}>关闭</Button>
            </DialogAction>
            <Snackbar  open={noticeOpenState} onClose={setClose} message={noticeMsg} autoHideDuration={3000} />
        </Dialog>
        
    </>
    );
}

function RenderStatusZone(hookEvent, className, style, vaild, name, avatar){    

    if(vaild){
        ReactDOM.render(
            <>
                <div className={style.userZone}>
                    <div className={style.userId}>
                        <Link href="/user/?id=user" underline="none" >{name}</Link>
                    </div>
                    <Avatar src={avatar}/>
                </div>
            </>
        , document.getElementsByClassName(className)[0]);
    }
    else{
        ReactDOM.render(LoginRegion(hookEvent, className, style),document.getElementsByClassName(className)[0]);
    }
}; 

function StatusZone (className){
    const [fuck, shit] = React.useState(false);
    var _useState = React.useState(false);
    noticeOpenState = _useState[0];
    NoticeOpenStateModifier = _useState[1];

    _useState = React.useState("notice");
    noticeMsg = _useState[0];
    noticeMsgModifier = _useState[1];

    _useState = React.useState("success");
    noticeType = _useState[0];
    noticeTypeModifier = _useState[1];

    const style = statusStyle();
    var request = new XMLHttpRequest();
    request.onreadystatechange = function(){
        if (request.readyState === 4 && request.status === 200)
        {
            try{
                var data = JSON.parse(request.responseText);
                RenderStatusZone([fuck, shit], className, style, data.vaild, data.name, data.avatar);
            }
            catch{
                RenderStatusZone([fuck, shit], className, style, false);
            }
            
            
        }
        else if (request.readyState === 4){
            RenderStatusZone([fuck, shit], className, style, false);
        }
    }
    //    return [false, "LegendLXH", "lxhcat.jpg"];
    request.open("GET", "/api/v1/GetUserInfo", true)
    request.send(null);

    return "";
};


const IndexZone = () => {
    const style = indexStyle();
    const [value, setValue] = React.useState("文章");

    function handleChange(event, newValue) {
        setValue(newValue);
        if (newValue === '首页')
        {
            window.location = '/';
        }
    };

    return(
        <Tabs value = {value} onChange={handleChange} classes={{indicator:style.indicatorColor}} >
            <Tab className="indexstyle-tabmod" label="首页"  value="首页"/>
            <Tab className="indexstyle-tabmod" label="文章"  value="文章" />
        </Tabs>
    );
};

const NavBar = () => {

    return(
        <AppBar position="fixed" style={{minWidth: 960, background: "#f5f5f5"}}>
            
            <ToolBar >
                <Typography className ="navstyle-title" variant="h5">
                    LXH's Blog Space
                </Typography>
                <div className="navstyle-index">
                    <IndexZone />
                </div>
                <div className="navstyle-status">
                    {StatusZone("navstyle-status")}
                </div>
            </ToolBar>
        </AppBar>
    )
};

export default NavBar;