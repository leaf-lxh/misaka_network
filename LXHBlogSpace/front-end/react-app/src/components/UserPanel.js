import React from 'react';
import ReactDOM from 'react-dom';

import Button from "@material-ui/core/Button"
import Chip from "@material-ui/core/Chip"
import "./css/UserPanel.css"

import NoteAdd from "@material-ui/icons/NoteAdd"
import ViewList from "@material-ui/icons/ViewList"
import History from "@material-ui/icons/History"




function RenderUserInfo(className){
    var request = new XMLHttpRequest();
    request.onreadystatechange = function(){
        if (request.readyState === 4 && request.status === 200)
        {
            var info = JSON.parse(request.responseText);
            ReactDOM.render(
                UserPanel(info),
                document.getElementsByClassName(className)[0]
            );
        }
        if (request.readyState === 4 && request.status !== 200)
        {
            info = {"ecode": -1, "username": "", "avatar": "", "system_msg": "--", "user_msg": "--", "account_state": "noLogin"};
            ReactDOM.render(
                UserPanel(info),
                document.getElementsByClassName(className)[0]
            );
        }
    }

    request.open("GET", "/api/v1/GetUserMsgInfo", true);
    request.send(null);
}


function UserPanel(userMsgInfo, className){
    if (userMsgInfo.ecode === undefined)
    {
        userMsgInfo = {"ecode": -1, "username": "", "avatar": "", "system_msg": "--", "user_msg": "--", "account_state": "noLogin"};
        RenderUserInfo("user-panel");
    }
    if (className === undefined)
    {
        className = "user-panel";
    }
    
    return(
        <>
            <div className="control-panel">
                <div className="control-elements">
                    <Button style={{display:"inline-block"}} href="/edit/">
                        <div className="control-elements-icon">
                            <NoteAdd />
                        </div>
                        <div className="control-elements-text">
                            创建文章
                        </div>
                    </Button>
                </div>
                <div className="control-elements">
                    <Button style={{display:"inline-block"}} href="/user/">
                        <div className="control-elements-icon">
                            <ViewList />
                        </div>
                        <div className="control-elements-text">
                            我的收藏
                        </div>
                    </Button>
                </div>
                <div className="control-elements">
                    <Button style={{display:"inline-block"}}>
                        <div className="control-elements-icon">
                            <History />
                        </div>
                        <div className="control-elements-text">
                            浏览历史
                        </div>
                    </Button>
                </div>
            </div>
            <div className="msg-panel">
                    <Chip className="msg-panel-elements" label={"系统消息: " + userMsgInfo["system_msg"]}/>
                    <Chip className="msg-panel-elements" label={"用户消息: " + userMsgInfo["user_msg"]}/>
            </div>
        </>
    )
}

export default UserPanel;