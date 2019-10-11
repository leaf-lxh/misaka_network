import React from 'react';
import ReactDOM from 'react-dom';

import Button from "@material-ui/core/Button"
import Chip from "@material-ui/core/Chip"
import "./css/UserPanel.css"

import NoteAdd from "@material-ui/icons/NoteAdd"
import ViewList from "@material-ui/icons/ViewList"
import ExitToApp from "@material-ui/icons/ExitToApp"

class UserPanel extends React.Component {
    
    constructor(props)
    {
        super(props)
        this.state = {
            logined: false,
            username: null,
            usermsg: "---",
            sysmsg: "---"
        }
    }
    
    clickHandler(type)
    {
        if (this.state.logined === false)
        {
            alert("请先登录");
            return;
        }

        if (type === "edit")
        {
            window.open("/edit/")
        }
        else if (type === "space")
        {
            window.open("/member/" + this.state.username);
        }
        else if (type === "logout")
        {
            fetch("/api/v1/passport/logout", {
                "method": "POST",
                "credentials": "include"
            })
            window.location = "/";
        }
        else if (type === "sysmsg")
        {
            window.open("/member/" + this.state.username + "#sysmsg");
        }
        else if (type === "usermsg")
        {
            window.open("/member/" + this.state.username + "#usermsg");
        }
    }

    render()
    {
        return(
        <>
            <div className="control-panel">
                <div className="control-elements">
                    <Button style={{display:"inline-block"}} onClick={this.clickHandler.bind(this,"edit")}>
                        <div className="control-elements-icon">
                            <NoteAdd />
                        </div>
                        <div className="control-elements-text">
                            创建文章
                        </div>
                    </Button>
                </div>
                <div className="control-elements">
                    <Button style={{display:"inline-block"}} onClick={this.clickHandler.bind(this,"space")}>
                        <div className="control-elements-icon">
                            <ViewList />
                        </div>
                        <div className="control-elements-text">
                            我的空间
                        </div>
                    </Button>
                </div>
                <div className="control-elements">
                    <Button style={{display:"inline-block"}} onClick={this.clickHandler.bind(this,"logout")}>
                        <div className="control-elements-icon">
                            <ExitToApp />
                        </div>
                        <div className="control-elements-text">
                            退出登录
                        </div>
                    </Button>
                </div>
            </div>
            <div className="msg-panel">
                    <Chip className="msg-panel-elements" label={"系统消息: " + this.state.sysmsg} onClick={this.clickHandler.bind(this,"sysmsg")}/>
                    <Chip className="msg-panel-elements" label={"用户消息: " + this.state.usermsg} onClick={this.clickHandler.bind(this,"usermsg")}/>
            </div>
        </>
        )
    }

    componentDidMount()
    {
        fetch("/api/v1/member/GetUnreadedMsgNum", {
            "credentials": "include"
        })
        .then(res=>{
            if (res.ok)
            {
                return res.json();
            }
        })
        .then(res=>{
            if (res !== undefined && res.ecode === "0")
            {
                this.setState({
                    username: window.user_info.username,
                    logined: true,
                    sysmsg: res.system_msgs,
                    usermsg: res.comment_msgs
                })
            }
        })
    }
}

export default UserPanel;
