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

import AccountBoxIcon from '@material-ui/icons/AccountBox'
import AssignmentIcon from '@material-ui/icons/Assignment'

import MainContainer from './MainContainer'
import "./css/NavBar.css"


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


function RenderStatusZone(className, style, vaild, name, avatar){    
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
        ReactDOM.render(
            <>
                <Button className={style.button} href="#login">
                    登录
                    <AccountBoxIcon className={style.icon} />
                </Button>
                <Button className={style.button} href="#join">
                    注册
                    <AssignmentIcon className={style.icon} />
                </Button>
            </>
        ,document.getElementsByClassName(className)[0]);
    }
}; 

function StatusZone (className){
    const style = statusStyle();
    var request = new XMLHttpRequest();
    request.onreadystatechange = function(){
        if (request.readyState === 4 && request.status === 200)
        {
            try{
                var data = JSON.parse(request.responseText);
                RenderStatusZone(className, style, data.vaild, data.name, data.avatar);
            }
            catch{
                RenderStatusZone(className, style, false);
            }
            
            
        }
        else if (request.readyState === 4){
            RenderStatusZone(className, style, false);
        }
    }
    //    return [false, "LegendLXH", "lxhcat.jpg"];
    request.open("GET", "/api/v1/GetUserInfo", true)
    request.send(null);

    return "";
};


const IndexZone = () => {
    const style = indexStyle();
    const [value, setValue] = React.useState("main");
    function handleChange(event, newValue) {
        setValue(newValue);
        ReactDOM.render(MainContainer("container-loading"), document.getElementById('main-panel-root'));
        ReactDOM.render(MainContainer(newValue), document.getElementById('main-panel-root'));
    };

    return(
        <Tabs value = {value} onChange={handleChange} classes={{indicator:style.indicatorColor}} >
            <Tab className="indexstyle-tabmod" label="首页"  value="main"/>
            <Tab className="indexstyle-tabmod" label="时间线"  value="timeline" />
            <Tab className="indexstyle-tabmod" label="深水池"  value="water"/>
            
        </Tabs>
    );
};

const NavBar = () => {

    return(
        <AppBar position="fixed" style={{minWidth: 960}}>
            <ToolBar className="navstyle-root">
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