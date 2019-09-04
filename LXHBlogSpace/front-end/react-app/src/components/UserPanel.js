import React from 'react';

import Button from "@material-ui/core/Button"
import Chip from "@material-ui/core/Chip"
import "./css/UserPanel.css"

import NoteAdd from "@material-ui/icons/NoteAdd"
import ViewList from "@material-ui/icons/ViewList"
import History from "@material-ui/icons/History"

function GetuserInfo(){
    var userInfoStr = '{"username": "TestName", "avatar": "lxhcat.jpg", "system_msg": "20", "user_msg": "30", "account_state": "ok"}';
    var userInfo = JSON.parse(userInfoStr);
    return userInfo;
}

const UserPanel = () => {
    var userInfo = GetuserInfo();
    return(
        <div className="user-panel">
            <div className="control-panel">
                <div className="control-elements">
                    <Button style={{display:"inline-block"}}>
                        <div className="control-elements-icon">
                            <NoteAdd />
                        </div>
                        <div className="control-elements-text">
                            创建文章
                        </div>
                    </Button>
                </div>
                <div className="control-elements">
                    <Button style={{display:"inline-block"}}>
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
                    <Chip className="msg-panel-elements" label={"系统消息: " + userInfo["system_msg"]}/>
                    <Chip className="msg-panel-elements" label={"用户消息: " + userInfo["user_msg"]}/>
            </div>
        </div>
    )
}

export default UserPanel;