import React from "react"
import Drawer from "@material-ui/core/Drawer"
import List from "@material-ui/core/List"
import ListItem from "@material-ui/core/ListItem"
import ListItemIcon from "@material-ui/core/ListItemIcon"
import AccountBox from "@material-ui/icons/AccountBox"
import Class from "@material-ui/icons/Class"
import Message from "@material-ui/icons/Message"
import Feedback from "@material-ui/icons/Feedback"
import VerticalSplit from "@material-ui/icons/VerticalSplit"
import History from "@material-ui/icons/History"

import ListItemText from "@material-ui/core/ListItemText"
import Container from "@material-ui/core/Container"
import Avatar from "@material-ui/core/Avatar"

import ExpansionPanel from "@material-ui/core/ExpansionPanel"
import ExpansionPanelSummary from "@material-ui/core/ExpansionPanelSummary"
import ExpansionPanelDetails from "@material-ui/core/ExpansionPanelDetails"

import Link from "@material-ui/core/Link"

import Input from "@material-ui/core/Input"
import Button from "@material-ui/core/Button"
import "./css/MainApp.css"

import BlogBriefBox from "./BlogBriefBox"
import Dialog from "@material-ui/core/Dialog"
import DialogTitle from "@material-ui/core/DialogTitle"
import DialogContent from "@material-ui/core/DialogContent"

class UserInfo extends React.Component
{
    constructor(props)
    {
        super(props)
        this.state = {
            avatarSrc: "default_avatar.jpg",
            avatarLoadSrc: null,
            followButtonLabel: "关注",
            userinfo: {username: "", current_user: "false"},
            dialogOpenState: false,
            dialogTitle: "null",
            dialogContent: <></>,
            fans_num: 0,
            followed_num: 0
        }
        this.followed = false;
        this.currentUsername = null;
    }

    InFollowListHandler(buttonId, targetName)
    {
        if (window.logined_username === undefined)
        {
            document.getElementById("open-login-button").click();
            return ;
        }

        var button = document.getElementById(buttonId);
        if (button.innerText === "取消关注")
        {
            fetch("/api/v1/member/RemoveFollow", {
                'method': "POST",
                "credentials": "include",
                "body": {
                    "Content-Type": "application/x-www-form/urlencoded"
                },
                "body": "username=" + targetName
            })
            .then(r=>{
                if (r.ok)
                {
                    return r.json();
                }
            })
            .then(r=>{
                if (r !== undefined && r.ecode === "0")
                {
                    button.innerText = "关注"
                    if (window.logined_username === this.currentUsername)
                    {
                        this.setState({
                            followed_num: this.state.followed_num - 1
                        })
                    }
                }
                else
                {
                    alert("取消关注失败，原因："+ r.reason);
                }
            })
        }
        else
        {
            fetch("/api/v1/member/AddFollow", {
                'method': "POST",
                "credentials": "include",
                "body": {
                    "Content-Type": "application/x-www-form/urlencoded"
                },
                "body": "username=" + targetName
            })
            .then(r=>{
                if (r.ok)
                {
                    return r.json();
                }
            })
            .then(r=>{
                if (r !== undefined && r.ecode === "0")
                {
                    button.innerText = "取消关注";
                    if (window.logined_username === this.currentUsername)
                    {
                        this.setState({
                            followed_num: this.state.followed_num + 1
                        })
                    }
                }
                else
                {
                    alert("关注失败，原因："+ r.reason);
                }
            })
        }
    }

    RenderFollowList(type, elementNum, page)
    {
        var pageParam = "";

        if (page !== undefined)
        {
            pageParam = "&page=" + page;
        }

        var url = "/api/v1/member/GetFollowerList?username=";
        if (type=== "followed")
        {
            url = "/api/v1/member/GetFollowedList?username=";
        }

        fetch(url + this.currentUsername + pageParam)
        .then(res=>{
            if (res.ok)
            {
                return res.json();
            }
        })
        .then(res=>{
            if (res === undefined)
            {
                return;
            }

            this.setState({
                dialogContent: 
                <>
                    {
                        function()
                        {
                            if (res.user_list.length === 0)
                            {
                                if (type === "follower")
                                {
                                    return (
                                        <div className="none-follow">
                                            Ta目前还没有粉丝~
                                        </div>
                                    )
                                }
                                else
                                {
                                    return (
                                        <div className="none-follow">
                                            Ta目前还没有关注其他用户~
                                        </div>
                                    )
                                }

                            }
                            else
                            {
                                return (
                                    <>
                                        <div className="follow-list">
                                            {
                                                res.user_list.map(function(element, index)
                                                {
                                                    return(
                                                        <div className="follow-info-block">
                                                            <a className="follow-info-user" href={"/member/" + element.username} target="_blank">
                                                                <Avatar className="follow-info-useravatar" src={"http://img-blog.leaflxh.com/" + element.avatar}/>
                                                                <div className="follow-info-username">{element.username}</div>
                                                            </a>
                                                            <div >
                                                                {
                                                                    function()
                                                                    {
                                                                        var text = "关注"
                                                                        if (element.followed === true)
                                                                        {
                                                                            text = "取消关注"
                                                                        }
                                                                        return <Button className="follow-info-button" id={"follower-index-" + index} onClick={this.InFollowListHandler.bind(this, "follower-index-" + index, element.username)}>{text}</Button>
                                                                    }.bind(this)()
                                                                }
                                                            </div>
                                                        </div>
                                                    )
                                                }.bind(this))
                                            }
                                        </div>
                                        <div className="follow-list-page">
                                        {
                                            function()
                                            {
                                                if (this.state.fans_num === 0)
                                                {
                                                    return;
                                                }
                                                
                                                //设定当前选中的页面，以及页数
                                                var currentPage = 1;
                                                var maxPage = 1;
                                                if (page !== undefined)
                                                {
                                                    currentPage = page;
                                                }
                                                maxPage = parseInt(elementNum / 9);
                                                if ((elementNum % 9) != 0)
                                                {
                                                    maxPage++;
                                                }

                                                if (maxPage === 1)
                                                {
                                                    return;
                                                }

                                                //准备生成页数列表
                                                var pageList = [];
                                                if (maxPage > 4)
                                                {
                                                    if (maxPage - currentPage > 2)
                                                    {
                                                        for (var index = 0; index <3; index++)
                                                        {
                                                            pageList[index] = currentPage + index//[1,2,3,'...', maxPage];
                                                        }
                                                        pageList.push("---");
                                                        pageList.push(maxPage);
                                                    }
                                                    else
                                                    {
                                                        pageList[0] = 1;
                                                        pageList[1] = "---";
                                                        for (index = 1; index <= 3; index++)
                                                        {
                                                            pageList.push(maxPage - (3-index));
                                                        }
                                                    }
                                                }
                                                else
                                                {
                                                    for (var i = 0; i < maxPage; ++i)
                                                    {
                                                        pageList[i] = i+1;
                                                    }
                                                }

                                                if (currentPage !== 1)
                                                {
                                                    pageList.push("上一页");
                                                }
                                                if (currentPage !== maxPage )
                                                {
                                                    pageList.push("下一页");
                                                }

                                                return pageList.map(pageNum=>{
                                                    if(pageNum === "下一页")
                                                    {
                                                        return <Button className="page-element" onClick={this.RenderFollowList.bind(this, type=type, elementNum=elementNum, page=currentPage+1)}>{pageNum}</Button>
                                                    }
                                                    if(pageNum === "上一页")
                                                    {
                                                        return <Button className="page-element" onClick={this.RenderFollowList.bind(this, type=type, elementNum=elementNum, page=currentPage-1)}>{pageNum}</Button>
                                                    }

                                                    if (pageNum === currentPage)
                                                    {
                                                        return <Button className="follow-list-current-page page-element">{pageNum}</Button>
                                                    }
                                                    if (pageNum === "---")
                                                    {
                                                        return <Button className="follow-list-omitted-page page-element">{pageNum}</Button>
                                                    }
                                                    return <Button className="follow-list-other-page page-element" onClick={this.RenderFollowList.bind(this, type=type, elementNum=elementNum, page=pageNum)}>{pageNum}</Button>
                                                })

                                            }.bind(this)()
                                        }
                                    </div>
                                    </>
                                )
                            }
                        }.bind(this)()
                    }
                </>
            })
        })
    }

    ShowFollower(page)
    {
        this.setState({
            dialogOpenState: true,
            dialogTitle: "粉丝列表",
            dialogContent: <>列表加载中</>
        })

        this.RenderFollowList.bind(this,"follower", this.state.fans_num)();
        
    }

    ShowFollowed()
    {
        this.setState({
            dialogOpenState: true,
            dialogTitle: "关注列表",
            dialogContent: <>列表加载中</>
        })

        this.RenderFollowList.bind(this,"followed", this.state.followed_num)();
    }

    render()
    {
        return (
            <Container fixed className="main-app-container">
                <div className="main-app-user-info">
                    <div className="avatar-region">
                        <input id="avatar-changer-input" type="file" style={{display: "none"}} />
                        <Avatar src={this.state.avatarLoadSrc} style={{width: 150, height:150, borderRadius: 4}}></Avatar>
                        <a className="change-avatar" href="#" onClick={this.ChangeAvatar.bind(this)}>更换头像</a>
                    </div>
                    <div className="main-app-user-brief">
                        <div className="main-app-user-brief-username" >{this.state.userinfo.username}</div>
                        <Input id="user-description-input" disableUnderline readOnly={!(this.state.userinfo.current_user==="true")} className="main-app-user-brief-desciption" placeholder={this.state.userDescription} onBlur={this.UpdateDescription.bind(this)} onFocus={this.SetDescription.bind(this)}/>
                    </div>
                    <div className="main-app-follow-info">
                        <Button className="main-app-follow-info-button" onClick={this.ShowFollowed.bind(this)}>关注了 {this.state.followed_num}</Button>
                        <Button className="main-app-follow-info-button" onClick={this.ShowFollower.bind(this)}>关注者 {this.state.fans_num}</Button>
                        <Dialog maxWidth={false} open={this.state.dialogOpenState} onClose={function(){this.setState({dialogOpenState: false})}.bind(this)}>
                            <DialogTitle>{this.state.dialogTitle}</DialogTitle>
                            <DialogContent className="follow-dialog">
                                {this.state.dialogContent}
                            </DialogContent>
                        </Dialog>
                    </div>
                </div>
                <div className="main-app-toolbar">
                    <div className="main-app-label">
                        文章
                    </div>
                    <Button className="subscribe-button" style={this.state.userinfo.current_user==="true" ? {display: "none"}: {}} onClick={this.PanelFollowHandler.bind(this)}>{this.state.followButtonLabel}</Button>
                </div>
                <div className="main-app-user-article">
                    {
                        this.state.srcArticleList
                    }
                </div>
            </Container>
        )
    }

    componentDidMount()
    {
        var username = window.location.pathname.match(/\/member\/(.+)/);
        document.title = username + " - LXHBlogSpace"
        if (username !== null)
        {
            username = username[1];
        }
        else
        {
            alert("指定用户不存在");
            window.location = "/";
            return;
        }
        this.currentUsername = username;

        fetch("/api/v1/passport/GetUserInfo?username=" + username)
        .then(response=>{
            if (response.status === 200)
            {
                return response.json();
            }
            else
            {
                alert("服务器未响应，请稍后再试")
                return;
            }
        })
        .then(userinfo=>{
            if (userinfo.ecode === "0")
            {
                fetch("/api/v1/content/GetUserArticleList?username=" + username)
                .then(response=>{
                    if (response.status === 200)
                    {
                        return response.json();
                    }
                    else
                    {
                        alert("服务器未响应，请稍后再试")
                        return;
                    }
                })
                .then(response=>{
                    if (response !== [])
                    {                       
                        fetch("/api/v1/member/GetUserFollowInfo?username=" + encodeURI(username), {"credentials": "include"})
                        .then(r=>r.json())
                        .then(followInfo=>{
                            if (followInfo.following === "true")
                            {
                                this.followed = true;
                                this.setState({
                                    followButtonLabel: "取消关注"
                                });
                            }

                            if (userinfo.current_user === "true")
                            {
                                if (userinfo.description === "")
                                {
                                    userinfo.description = "点击此处添加个人简介"
                                }
                                document.getElementsByClassName("change-avatar")[0].style.display = "inherite";
                            }
                            else
                            {
                                document.getElementsByClassName("change-avatar")[0].style.display = "none";
                            }

                            if (response.length === 0)
                            {
                                this.setState({
                                    srcArticleList:
                                    <div style={{paddingTop: 20, paddingBottom:20, textAlign:"center"}}>
                                        该用户暂未发表文章
                                    </div>
                                })
                            }
                            else
                            {
                                this.setState({
                                    srcArticleList: response.map((article,index)=>{
                                        return BlogBriefBox(window.decodeURIComponent(escape(window.atob(article.title))), window.decodeURIComponent(escape(window.atob(article.brief))), [], {"vote":article.vote_num, "comments":article.comment_num}, [], article.article_id, article.create_date )
                                    })
                                })
                            }

                            this.setState({
                                avatarSrc: userinfo.avatar,
                                avatarLoadSrc: "http://img-blog.leaflxh.com/" + userinfo.avatar, 
                                userDescription: userinfo.description,
                                userinfo: userinfo,
                                fans_num: parseInt(followInfo.fans_num), 
                                followed_num: parseInt(followInfo.followed_num)
                            })
                        })
                    }
                })
            }
            else
            {
                alert("查询失败，原因：" + userinfo.reason);
                window.location = "/";
            }
        })
    }

    ChangeAvatar()
    {
        var avatarInput = document.getElementById("avatar-changer-input");
        avatarInput.click();
        avatarInput.onchange = function(){
            var file = avatarInput.files[0];
            if (file.type !== "image/png" && file.type !== "image/jpeg" && file.type !== "image/bmp")
            {
                alert("仅允许使用png, jpeg或bmp格式的图片");
                return;
            }
            
            fetch("/api/v1/content/UploadImage", {
                method: "POST",
                credentials: "include",
                body: avatarInput.files[0]
            })
            .then(res=>{
                if (res.ok === false)
                {
                    alert("服务器未响应，上传失败");
                    return;
                }
                else
                {
                    return res.json();
                }
            })
            .then(function(response){
                console.log(this)
                if (response === undefined)
                {    
                    return;
                }

                if (response.ecode === "0")
                {
                    fetch("/api/v1/passport/UpdateUserDetails", {
                        "method":"POST",
                        "credentials":"include",
                        "headers":{
                            "Content-Type": "application/x-www-form/urlencoded"
                        },
                        "body": "avatar_path=" + encodeURI(response.img_path) + "&description=" + encodeURI(this.state.userDescription)
                    })
                    .then(r=>{
                        if (r.ok)
                        {
                            return r.json();
                        }
                        else
                        {
                            alert("更换头像失败，请稍后再试");
                        }
                    })
                    .then(r=>{
                        if (r !== undefined && r.ecode === "0")
                        {
                            this.setState({
                                avatarSrc: response.img_path
                            })

                            var avatars = document.getElementsByClassName("MuiAvatar-img");
                            for (var i = 0; i < avatars.length; ++i)
                            {
                                avatars[i].src = this.state.avatarSrc;
                            }
                        }
                    })
                }
                else
                {
                    alert("上传失败，原因：" + response.reason);
                }
                return;
            }.bind(this))
        }.bind(this)
    }

    PanelFollowHandler()
    {
        if (window.logined_username === undefined)
        {
            document.getElementById("open-login-button").click();
            return;
        }

        if (this.followed === true)
        {
            fetch("/api/v1/member/RemoveFollow", {
                'method': "POST",
                "credentials": "include",
                "body": {
                    "Content-Type": "application/x-www-form/urlencoded"
                },
                "body": "username=" + this.currentUsername
            })
            .then(r=>{
                if (r.ok)
                {
                    return r.json();
                }
            })
            .then(r=>{
                if (r !== undefined && r.ecode === "0")
                {
                    this.followed = false;
                    this.setState({
                        fans_num: this.state.fans_num - 1,
                        followButtonLabel: "关注"
                    })
                }
                else
                {
                    alert("取消关注失败，原因："+ r.reason);
                }
            })
        }
        else
        {
            fetch("/api/v1/member/AddFollow", {
                'method': "POST",
                "credentials": "include",
                "body": {
                    "Content-Type": "application/x-www-form/urlencoded"
                },
                "body": "username=" + this.currentUsername
            })
            .then(r=>{
                if (r.ok)
                {
                    return r.json();
                }
            })
            .then(r=>{
                if (r !== undefined && r.ecode === "0")
                {
                    this.followed = true;
                    this.setState({
                        fans_num: this.state.fans_num + 1,
                        followButtonLabel: "取消关注"
                    })
                }
                else
                {
                    alert("关注失败，原因："+ r.reason);
                }
            })
        }
    }

    SetDescription()
    {
        //do login check
        var input = document.getElementById("user-description-input");
        if (input.placeholder !== "点击此处添加个人简介")
        {
            input.value = this.state.userDescription;
        }
    }

    UpdateDescription()
    {
        console.log(this)
        if (this.state.userinfo.current_user === "true")
        {
            fetch("/api/v1/passport/UpdateUserDetails", {
                "method":"POST",
                "credentials":"include",
                "headers":{
                    "Content-Type": "application/x-www-form/urlencoded"
                },
                "body": "avatar_path=" + encodeURI(this.state.avatarSrc) + "&description=" + encodeURI(document.getElementById("user-description-input").value)
            })
            .then(r=>{
                if (r.ok)
                {
                    return r.json();
                }
                else
                {
                    alert("设置个人描述失败，请稍后再试");
                }
            })
            .then(r=>{
                if (r !== undefined && r.ecode === "0")
                {
                    this.setState({
                        userDescription: document.getElementById("user-description-input").value
                    })
                    document.getElementById("user-description-input").value = "";
                }
            })
            
        }
    }
}

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
            selectedDraft:"",
            selectedHistory: "",
            srcContainer: <></>,
            srcArticleList: <></>,
            paddingState: "none",
            followButtonLabel: "关注",
            avatarSrc: "img-blog.leaflxh.com/images/default_avatar.jpg"
        };
        this.currentLoginUser = false;
        this.avatarChangable = "none"
        this.followed = false;
        this.currentUsername = null;
    }


    RenderUserInfo()
    {
        this.setState({
            srcContainer: <UserInfo/>
        })
    }
    HandleReadMsg(event, state, type, notice_id)
    {
        if (state === false)
        {
            return;
        }

        document.getElementById("msg-" + notice_id).style.fontWeight = "400";
        document.getElementById("msg-" + notice_id).style.background = "white";
        fetch("/api/v1/member/SetMessageReaded", {
            'method': 'POST',
            'credentials': 'include',
            'headers': {
                'Content-Type': "application/x-www-form/urlencoded"
            },
            'body': "type=" + type + "&notice_id=" + notice_id 
        })
    }

    

    RenderSystemNotice()
    {
        fetch("/api/v1/member/GetSystemMsgList", {"credentials":"include"})
        .then(r=>r.json())
        .then(res=>{
            console.log(res.msg_list.length === 0)
            if (res.msg_list.length === 0)
            {
                this.setState({
                    srcContainer:
                    <Container fixed className="main-app-container none-notice">
                        暂无系统消息
                    </Container>
                })
                return;
            }
            
            this.setState({srcContainer:<></>});
            this.setState({
                srcContainer: 
                    <Container fixed className="msg-container" >
                        {
                            res.msg_list.map((msg, index)=>{
                                var time = new Date(parseInt(msg.time) * 1000);
                                var additionalStyle = "";
                                if (msg.readed === 0)
                                {
                                    additionalStyle = "msg-panel-unreaded"
                                }
                                return (
                                    <ExpansionPanel id={"msg-"+msg.notice_id} className={"msg-panel "+ additionalStyle} onChange={(e,s)=>this.HandleReadMsg(e, s, "system", msg.notice_id)}>
                                        <ExpansionPanelSummary>
                                            {msg.title}
                                        </ExpansionPanelSummary>
                                        <ExpansionPanelDetails className="msg-panel-details">
                                            {time.toLocaleDateString() + " " + time.toLocaleTimeString('zh-CN', {hour12: false})}
                                            <br/>
                                            {msg.content}
                                        </ExpansionPanelDetails>
                                    </ExpansionPanel>
                                )
                            })
                        }
                    </Container>
                
            })
        })
    }

    RenderUserCommentNotice()
    {
        fetch("/api/v1/member/GetUserMsgList", {"credentials":"include"})
        .then(r=>r.json())
        .then(res=>{
            console.log(res.msg_list.length === 0)
            if (res.msg_list.length === 0)
            {
                this.setState({
                    srcContainer:
                    <Container fixed className="main-app-container none-notice">
                        暂无用户评论
                    </Container>
                })
                return;
            }
            this.setState({srcContainer:<></>});
            this.setState({
                srcContainer: 
                    <Container fixed className="msg-container" >
                        {
                            res.msg_list.map((msg, index)=>{
                                var time = new Date(parseInt(msg.time) * 1000);
                                var additionalStyle = "";
                                if (msg.readed === "0")
                                {
                                    additionalStyle = "msg-panel-unreaded"
                                }
                                return (
                                    <ExpansionPanel id={"msg-"+msg.notice_id} className={"msg-panel "+ additionalStyle} onChange={(e,s)=>this.HandleReadMsg(e, s, "user", msg.notice_id)}>
                                        <ExpansionPanelSummary>
                                            用户 {msg.from} 在你的文章 {msg.title} 进行了评论
                                        </ExpansionPanelSummary>
                                        <ExpansionPanelDetails className="msg-panel-details">
                                            {time.toLocaleDateString() + " " + time.toLocaleTimeString('zh-CN', {hour12: false})}
                                            <br/>
                                            {msg.content}
                                        </ExpansionPanelDetails>
                                    </ExpansionPanel>
                                )
                            })
                        }
                    </Container>
                
            })
        })

    }

    Unsubscribe(article_id)
    {
        fetch("/api/v1/content/subscribe",{
            "method": "POST",
            "credentials":"include",
            "headers":{
                "Content-Type": "application/x-www-form/urlencoded"
            },
            "body": "action=cancle&article_id=" + article_id
        })
        .then(r=>{
            if (r.ok)
            {
                return r.json();
            }
            else if (r.status === 403)
            {
                alert("请先登录");
                return;
            }
            else
            {
                alert("服务器未响应，请稍后再试");
                return;
            }
        })
        .then(response=>{
            if (response !== undefined && response.ecode === "0")
            {
                window.location.reload();
            }
            else
            {
                alert("操作失败，原因：" + response.reason);
            }
        })
    }

    RenderMyCollection()
    {
        fetch("/api/v1/content/GetSubscribedList", {"credentials":"include"})
        .then(r=>r.json())
        .then(res=>{
            console.log(res.length === 0)
            if (res.length === 0)
            {
                this.setState({
                    srcContainer:
                    <Container fixed className="main-app-container none-notice">
                        暂无收藏
                    </Container>
                })
                return;
            }
            this.setState({srcContainer:<></>});
            this.setState({
                srcContainer: 
                    <Container fixed className="msg-container" >
                        {
                            res.map((collection, index)=>{
                                var time = new Date(collection.add_time * 1000);
                                return (
                                    <ExpansionPanel className="msg-panel" expanded={true} onChange={()=>{window.open("/blogs/" + collection.article_id)}}>
                                        <ExpansionPanelSummary style={{color: "#3f51b5", fontWeight: 700, fontSize: 18}}>
                                            {collection.title}
                                        </ExpansionPanelSummary>
                                        <ExpansionPanelDetails className="msg-panel-details" style={{display: "block"}}>
                                            <div style={{display: "flex"}}>
                                                <div>{"收藏时间：" + time.toLocaleDateString() + " " + time.toLocaleTimeString('zh-CN', {hour12: false})}</div>
                                                <Button onClick={()=>this.Unsubscribe(collection.article_id)} style={{background: "rgb(122, 176, 202)", color:"white", marginLeft: "auto"}}>取消收藏</Button>
                                            </div>
                                            <div style={{display: "flex", paddingTop: 5}}>
                                                作者：
                                                <Link href={"/member/" + collection.author}>{collection.author}</Link>
                                            </div>
                                        </ExpansionPanelDetails>
                                    </ExpansionPanel>
                                )
                            })
                        }
                    </Container>
                
            })
        })
    }

    DeleteDraft(draft_id)
    {
        if (window.confirm("确认删除吗，此操作不可逆转"))
        {
            fetch("/api/v1/content/DeleteDraft",{
                'method':'POST',
                "credentials": 'include',
                'headers':{
                    'Content-Type': "application/x-www-form/urlencoded"
                },
                'body':"draft_id="+ draft_id
            })
            .then(res=>{
                if(res.ok)
                {
                    return res.json()
                }
                else
                {
                    alert("服务器未响应，请稍后再试");
                    return;
                }
            })
            .then(res=>{
                if (res !== undefined && res.ecode === "0")
                {
                    //document.getElementById("draft-" + draft_id).style.display = "none";
                    window.location = window.location;
                }
                else
                {
                    alert("删除失败，原因：" + res.reason);
                }
            })
        }
    }

    RenderMyDrafts()
    {
        fetch("/api/v1/content/GetDraftList", {"credentials":"include"})
        .then(res=>{
            if(res.ok)
            {
                return res.json();
            }
            else
            {
                alert("服务器未响应，请稍后再试");
                return ;
            }
        })
        .then(res=>{
            if (res !== undefined)
            {
                this.setState({
                    srcContainer: <></>
                })
                
                if (res.length !== 0)
                {
                    this.setState({
                        srcContainer:
                        <Container fixed className="msg-container" >
                            {
                                res.map((draft, index)=>{
                                    var time = new Date(draft.lastmodify_date * 1000);
                                    console.log(draft);
                                    return (
                                        <ExpansionPanel id={"draft-"+draft.draft_id} className="msg-panel" expanded={true} onChange={()=>{window.open("/edit?draft=" +draft.draft_id)}}>
                                            <ExpansionPanelSummary style={{color: "#3f51b5", fontWeight: 700, fontSize: 18}}>
                                                标题：{draft.title}
                                            </ExpansionPanelSummary>
                                            <ExpansionPanelDetails className="msg-panel-details" style={{display: "block"}}>
                                                <div style={{display:"flex"}}>
                                                    <div>{"保存时间：" + time.toLocaleDateString() + " " + time.toLocaleTimeString('zh-CN', {hour12: false})}</div>
                                                    <Button onClick={()=>this.DeleteDraft(draft.draft_id)} style={{background: "#ff4700", color:"white", marginLeft: "auto"}}>删除</Button>
                                                </div>
                                                <div style={{display: "flex", paddingTop: 5}}>
                                                    摘要：
                                                    <br/>
                                                    {draft.content}
                                                </div>
                                            </ExpansionPanelDetails>
                                        </ExpansionPanel>
                                    )
                                })
                            }
                        </Container>
                    })
                }
                else
                {
                    this.setState({
                        srcContainer:
                        <Container fixed className="main-app-container none-notice">
                            暂无草稿
                        </Container>
                    })
                }
                
            }
        })
    }

    RenderMyHistory()
    {

    }

    switchContainer(page)
    {
        this.setState({
            selectedUserInfo: "",
            selectedSystemMsg: "",
            selectedMyMsg: "",
            selectedMyFavorite: "",
            selectedDraft: "",
            selectedHistory: ""
        });
        
        if (page === "个人资料")
        {
            window.location.hash = "";
            this.setState({
                selectedUserInfo: "panel-selected-button",
            })
            this.RenderUserInfo()
        }
        else if (page === "系统通知")
        {
            window.location.hash = "#sysmsg";
            this.setState({
                selectedSystemMsg: "panel-selected-button"
            })
            this.RenderSystemNotice()
        }
        else if (page === "我的消息")
        {
            window.location.hash = "#usermsg";
            this.setState({
                selectedMyMsg: "panel-selected-button"
            })
            this.RenderUserCommentNotice()
        }
        else if (page === "我的收藏")
        {
            window.location.hash = "#collection";
            this.setState({
                selectedMyFavorite: "panel-selected-button"
            })
            this.RenderMyCollection();
        }
        else if (page === "我的草稿")
        {
            window.location.hash = "draftlist";
            this.setState({
                selectedDraft: "panel-selected-button"
            })
            this.RenderMyDrafts();
        }
        else if (page === "浏览历史")
        {
            window.location.hash = "history";
            this.setState({
                selectedHistory: "panel-selected-button"
            })
            this.RenderMyHistory();
        }

    }

    render()
    {
        var selectedState = [this.state.selectedUserInfo, this.state.selectedSystemMsg, this.state.selectedMyMsg, this.state.selectedMyFavorite, this.state.selectedDraft, this.state.selectedHistory]
        var icons = [<AccountBox />, <Feedback />, <Message />, <Class />,<VerticalSplit/>, <History/>]
        var buttons = ["个人资料", "系统通知", "我的消息", "我的收藏", "我的草稿"]//, "浏览历史"]

        return (
            <>
                <Drawer variant="persistent" className="left-drawer" open={this.state.currentLoginUser} style={{width: 280, display:this.state.paddingState}}>
                    <div className="toolbar-padding"></div>
                    <List style={{width: 280}}>
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
                <div className="drawer-padding" style={{display:this.state.paddingState}}/>
                {this.state.srcContainer}
            </>
            
        )
    }

    componentDidMount()
    {
        
        fetch("/api/v1/passport/IsLogin", {'credentials':'include'})
        .then(res=>{
            if (res.ok)
            {
                return res.json()
            }
        })
        .then(res=>{
            if (res === undefined || res.vaild === "false")
            {
                this.switchContainer("个人资料");
                return;
            }
            var bodyNode = document.getElementsByTagName("body")[0];

            var username = window.location.pathname.match(/\/member\/(.+)/);
            if (username !== null)
            {
                username = username[1];
            }

            if (res.username === username)
            {
                this.setState({
                    paddingState: "inline",
                    currentLoginUser: true
                })
                bodyNode.style.backgroundPositionX = '280px';

                if (window.location.hash === "#sysmsg")
                {
                    this.switchContainer("系统通知");
                }
                else if (window.location.hash === "#usermsg")
                {
                    this.switchContainer("我的消息");
                }
                else if (window.location.hash === "#collection")
                {
                    this.switchContainer("我的收藏");
                }
                else if (window.location.hash === "#draftlist")
                {
                    this.switchContainer("我的草稿");
                }
                else if (window.location.hash === "#history")
                {
                    this.switchContainer("浏览历史");
                }
                else
                {
                    this.switchContainer("个人资料");
                }
            }
            else
            {
                this.switchContainer("个人资料");
            }
        })        
    }

}

export default MainApp;