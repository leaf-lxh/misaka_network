import React from "react"
import Drawer from "@material-ui/core/Drawer"
import List from "@material-ui/core/List"
import ListItem from "@material-ui/core/ListItem"
import ListItemIcon from "@material-ui/core/ListItemIcon"
import AccountBox from "@material-ui/icons/AccountBox"
import Class from "@material-ui/icons/Class"
import Message from "@material-ui/icons/Message"
import Feedback from "@material-ui/icons/Feedback"

import ListItemText from "@material-ui/core/ListItemText"
import Container from "@material-ui/core/Container"

import "./css/MainApp.css"
class MainApp extends React.Component
{
    constructor(props)
    {
        super(props)
        this.state = {
            selectedUserInfo: "panel-selected-button",
            selectedSystemMsg: "",
            selectedMyMsg: "",
            selectedMyFavorite: "",
            srcContainer: <></>
        };
    }

    userInfo()
    {
        return (
            <Container fixed className="main-app-container">
                个人资料
            </Container>
        )
    }

    switchContainer(page)
    {
        this.setState({
            selectedUserInfo: "",
            selectedSystemMsg: "",
            selectedMyMsg: "",
            selectedMyFavorite: ""
        });
        if (page === "个人资料")
        {
            this.setState({
                selectedUserInfo: "panel-selected-button",
                srcContainer: this.userInfo()
            })
        }
        else if (page === "系统通知")
        {
            this.setState({
                selectedSystemMsg: "panel-selected-button"
            })
        }
        else if (page === "我的消息")
        {
            this.setState({
                selectedMyMsg: "panel-selected-button"
            })
        }
        else if (page === "我的收藏")
        {
            this.setState({
                selectedMyFavorite: "panel-selected-button"
            })
        }

    }
    render()
    {
        var selectedState = [this.state.selectedUserInfo, this.state.selectedSystemMsg, this.state.selectedMyMsg, this.state.selectedMyFavorite]
        var icons = [<AccountBox />, <Feedback />, <Message />, <Class />]
        var buttons = ["个人资料", "系统通知", "我的消息", "我的收藏", "我的草稿", "浏览历史"]
        return (
            <>
                <Drawer variant="permanent">
                    <div className="toolbar-padding"></div>
                    <List>
                        {buttons.map((buttonText, index)=>{
                                return (
                                    <>
                                        <ListItem button className={"panel-list-item " + selectedState[index]} key={buttonText} onClick={func=>this.switchContainer(buttonText)}>
                                            <ListItemIcon style={{minWidth: 0}}>{icons[index]}</ListItemIcon>
                                            <ListItemText style={{paddingLeft: "15px"}} primary={buttonText} />
                                        </ListItem>
                                    </>
                                )
                        })}
                    </List>
                </Drawer>
                {this.state.srcContainer}
            </>
            
        )
    }

    componentDidMount()
    {
        this.switchContainer("个人资料");
    }

}

export default MainApp;