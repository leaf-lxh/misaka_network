import React from 'react';
// eslint-disable-next-line
import { makeStyles } from '@material-ui/core/styles'

import Button from "@material-ui/core/Button"
import "./css/UserPanel.css"

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
                <div>
                    <Button>创建文章</Button>
                </div>
            </div>
            <div className="msg-panel">
                {userInfo["system_msg"]}
            </div>
        </div>
    )
}

export default UserPanel;