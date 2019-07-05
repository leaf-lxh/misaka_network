import React from 'react'
import { makeStyles } from '@material-ui/core/styles'
import AppBar from '@material-ui/core/AppBar'
import ToolBar from '@material-ui/core/Toolbar'
import Typograph from '@material-ui/core/Typography'
import Button from '@material-ui/core/Button'

import AccountBoxIcon from '@material-ui/icons/AccountBox'
import AssignmentIcon from '@material-ui/icons/Assignment'

const navStyle = makeStyles ({
    navbar : 
    {
        background : "#f5f5f5"
    },
    title :
    {
        flexGrow : 1,
        color : "black",
        paddingLeft : "3%"
    },
    zone : 
    {
        position: 'relative',
        marginLeft: 0,
        width: '100%'
    }
})

const statusStyle = makeStyles({
    button : 
    {
        padding : "5px 50px 5px 50px ",
        background : "#e0e0e0",
        marginLeft : "8px",
        "&:hover" : 
        {
            background : "#d2d0d0"
        },
    },
    icon : 
    {
        paddingLeft : "5px",
    }
})

function isLoggedIn(){
    return false;
}

const StatusZone = () =>{
    const style = statusStyle();
    if(isLoggedIn())
    {
        return(
            <div>You are logged in.</div>
        )
    }
    else
    {
        return(
            <>
                <Button className={style.button} href="javascript:{alert('这是登录');}">
                    登录
                    <AccountBoxIcon className={style.icon} />
                </Button>
                <Button className={style.button} href="javascript:{alert('这是注册');}">
                    注册
                    <AssignmentIcon className={style.icon} />
                    
                </Button>
            </>
        )
    }
}
const NavBar = () => {
    const style = navStyle();
    return(
        <AppBar >
            <ToolBar className={style.navbar}>
                <Typograph className = {style.title} variant="h5">
                    LXH's Blog Space
                </Typograph>
                <div className={style.none}>
                    <StatusZone />
                </div>
            </ToolBar>
        </AppBar>
    )
};

export default NavBar;