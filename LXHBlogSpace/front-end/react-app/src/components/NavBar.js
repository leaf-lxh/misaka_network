import React from 'react';
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


const navStyle = makeStyles ({
    root : {
        background : "#f5f5f5"
    },
    title :{
        color : "black",
        marginLeft : "3%"
    },
    index : {
        marginLeft : "200px",
        flexGrow : 1
    },
    status : {
        
    }

});

const indexStyle = makeStyles({
    button : {
        margin : "0 10px 0 10px",
        borderRadius : 0,
        "&:hover" : {
            background : "#f5f5f5"
        }
    },
    tabMod : {
        marginTop : 8,
        marginLeft : 20,
        color : "black",
        fontSize: 16,
        minWidth : 0,
        minHeight : 20
    },
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


function isVaildLogin(){
    return [false, "LegendLXH", "lxhcat.jpg"];
}; 

const StatusZone = () =>{
    const style = statusStyle();
    const [vaild, name, avatar] = isVaildLogin();
    if(vaild){
        return(
            <>
                <div className={style.userZone}>
                    <div className={style.userId}>
                        <Link href="#user/?id=user" underline="none" >{name}</Link>
                    </div>
                    <Avatar src={avatar}/>
                </div>
            </>
        );
    }
    else{
        return(
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
        );
    }
};

const IndexZone = () => {
    const style = indexStyle();
    const [value, setValue] = React.useState(0);
    function handleChange(event, newValue) {
        setValue(newValue);
    };

    return(
        <Tabs value = {value} onChange={handleChange} classes={{indicator:style.indicatorColor}} >
            <Tab classes={{root:style.tabMod}} label="首页" href="#main" />
            <Tab classes={{root:style.tabMod}} label="时间线" href="#timeline" />
            <Tab classes={{root:style.tabMod}} label="深水池" href="#water" />
            
        </Tabs>
    );
};

const NavBar = () => {
    const style = navStyle();
    return(
        <AppBar >
            <ToolBar className={style.root}>
                <Typography className = {style.title} variant="h5">
                    LXH's Blog Space
                </Typography>
                <div className={style.index}>
                    <IndexZone />
                </div>
                <div className={style.status}>
                    <StatusZone />
                </div>
            </ToolBar>
        </AppBar>
    )
};

export default NavBar;