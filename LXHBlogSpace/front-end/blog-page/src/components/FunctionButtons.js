import React from 'react'
import Button from "@material-ui/core/Button"
import PlusOne from "@material-ui/icons/PlusOne"
import AddBox from "@material-ui/icons/AddBox"
import Message from "@material-ui/icons/Message"
import Edit from "@material-ui/icons/Edit"
import DeleteForever from "@material-ui/icons/DeleteForever"

import Dialog from "@material-ui/core/Dialog"
import DialogTitle from "@material-ui/core/DialogTitle"
import DialogAction from "@material-ui/core/DialogActions"
import DialogContent from "@material-ui/core/DialogContent"

import Input from "@material-ui/core/Input"
import Link from "@material-ui/core/Link"
import Snackbar from '@material-ui/core/Snackbar'

import Container from "@material-ui/core/Container"


import "./css/FunctionButtons.css"
import { Avatar } from '@material-ui/core'



class FunctionButtons extends React.Component
{
    constructor(props)
    {
        super(props);
        this.state = {
            voteNum: parseInt(window.blog_info.vote_num),
            commentNum: parseInt(window.blog_info.comment_num),
            dialogDisplayState: false,
            noticeDiaplayState: false,
            noticeMsg: "null",
            srcCommentBlock: <></>,

            voteButtonStyle:{},
            subscribeButtonStyle: {},
            editButtonStyle: {display:"none"},
            deleteButtonStyle: {display:"none"}
        }
        this.currentCommentPage = 1;
        this.maxCommentPage = 1;

        this.articleId = null;
        this.voted = false;
        this.subscribed = false;
    }


    ShowDialog()
    {
        this.setState({
            srcCommentBlock: <div>评论加载中</div>,
            dialogDisplayState: true
        })
        this.currentCommentPage = 1;
        this.maxCommentPage = 1;
        this.RetriveComment();
    }

    HideDialog()
    {
        this.setState({
            dialogDisplayState: false
        })
    }

    onClickSendComment()
    {
        var commentContent = document.getElementById("comment-input").value;
        if (commentContent === "")
        {
            this.setState({
                noticeDiaplayState: true,
                noticeMsg: "评论不能为空"
            })
            return;
        }

        fetch("/api/v1/content/SendComment", {
            "method": "POST",
            "credentials": "include",
            "header": {
                "Content-Type": "application/x-www-form/urlencoded"
            },
            "body": "article_id=" + this.articleId + "&content=" + window.btoa(unescape(window.encodeURIComponent(commentContent)))
        })
        .then(res=>{
            if (res.ok)
            {
                return res.json();
            }
            else if (res.status === 403)
            {
                this.setState({
                    noticeDiaplayState: true,
                    noticeMsg: "请先登录"
                })
                return;
            }
            else
            {
                this.setState({
                    noticeDiaplayState: true,
                    noticeMsg: "服务器未响应，请稍后再试"
                })
                return;
            }
        })
        .then(res=>{
            if (res !== undefined && res.ecode === "0")
            {
                document.getElementById("comment-input").value = "";
                this.RetriveComment()
                this.setState({
                    noticeDiaplayState: true,
                    noticeMsg: "发送成功"
                })
                return;
            }
            else
            {
                this.setState({
                    noticeDiaplayState: true,
                    noticeMsg: "发送失败，原因：" + res.reason
                })
                return;
            }
        })
    }

    HideNotice()
    {
        this.setState({
            noticeDiaplayState: false
        })
    }

    DeleteArticle()
    {
        if(window.confirm("确定删除本文吗？此操作不可恢复"))
        {
            fetch("/api/v1/content/DeleteArticle",{
                "method": "POST",
                "credentials": "include",
                "headers": {
                    "Content-Type": "application/x-www-form/urlencoded"
                },
                "body": "article_id=" + this.articleId
            })
            .then(r=>{
                if (r.ok)
                {
                    return r.json();
                }
                else if (r.status === 403)
                {
                    alert("无法删除，因为后端判断你权限不足")
                    return;
                }
            })
            .then(response=>{
                if (response !== undefined && response.ecode === "0")
                {
                    alert("删除成功");
                    window.location = "/";
                    return;
                }
                else
                {
                    alert("删除失败，原因：" + response.reason);
                }
            })
        }
    }

    EditArticle()
    {
        window.location = "/edit?article=" + this.articleId;
    }

    VoteHandler()
    {
        if (this.voted === false)
        {
            fetch("/api/v1/content/vote",{
                "method": "POST",
                "credentials":"include",
                "headers":{
                    "Content-Type": "application/x-www-form/urlencoded"
                },
                "body": "action=vote&article_id=" + this.articleId
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
                    this.voted = true;
                    this.setState({
                        voteButtonStyle: {background: "#9fd2ea"},
                        voteNum: this.state.voteNum  + 1
                    });
                }
                else
                {
                    alert("操作失败，原因：" + response.reason);
                }
            })
        }
        else
        {
            fetch("/api/v1/content/vote",{
                "method": "POST",
                "credentials":"include",
                "headers":{
                    "Content-Type": "application/x-www-form/urlencoded"
                },
                "body": "action=cancle&article_id=" + this.articleId
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
                    this.voted = false;
                    this.setState({
                        voteButtonStyle: {backgroundColor: "#f5f5f5"},
                        voteNum: this.state.voteNum  - 1
                    });
                }
                else
                {
                    alert("操作失败，原因：" + response.reason);
                }
            })
        }
    }

    SubscribeHandler()
    {
        if (this.subscribed === false)
        {
            fetch("/api/v1/content/subscribe",{
                "method": "POST",
                "credentials":"include",
                "headers":{
                    "Content-Type": "application/x-www-form/urlencoded"
                },
                "body": "action=subscribe&article_id=" + this.articleId
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
                    this.subscribed = true;
                    this.setState({
                        subscribeButtonStyle: {background: "#9fd2ea"},
                    });
                }
                else
                {
                    alert("操作失败，原因：" + response.reason);
                }
            })
        }
        else
        {
            fetch("/api/v1/content/subscribe",{
                "method": "POST",
                "credentials":"include",
                "headers":{
                    "Content-Type": "application/x-www-form/urlencoded"
                },
                "body": "action=cancle&article_id=" + this.articleId
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
                    this.subscribed = false;
                    this.setState({
                        subscribeButtonStyle: {backgroundColor: "#f5f5f5"},
                    });
                }
                else
                {
                    alert("操作失败，原因：" + response.reason);
                }
            })
        }
    }

    render()
    {
        return (
        <>
            <Button className="func-button" style={this.state.voteButtonStyle} onClick={this.VoteHandler.bind(this)} >
                <PlusOne />
                <div className="button-label">
                    {this.state.voteNum}&nbsp;赞同
                </div>
            </Button>
            <Button className="func-button" onClick={this.ShowDialog.bind(this)} style={{background: "#f5f5f5"}}>
                <Message />
                <div className="button-label">
                    {this.state.commentNum}&nbsp;评论
                </div>
            </Button>
            <Button className="func-button" style={this.state.subscribeButtonStyle} onClick={this.SubscribeHandler.bind(this)}>
                <AddBox />
                <div className="button-label">
                添加收藏
                </div>
            </Button>
            <Button className="func-button" style={this.state.editButtonStyle} onClick={this.EditArticle.bind(this)} >
                <Edit />
                <div className="button-label">
                修改文章
                </div>
            </Button>
            <Button className="func-button" style={this.state.deleteButtonStyle} onClick={this.DeleteArticle.bind(this)} >
                <DeleteForever />
                <div className="button-label">
                删除文章
                </div>
            </Button>
            <Dialog open={this.state.dialogDisplayState} onClose={this.HideDialog.bind(this)} maxWidth="false">
                <DialogTitle>本文章的评论区</DialogTitle>
                <DialogContent style={{borderTop: "1px solid gray", borderBottom: "1px solid gray", marginLeft: "5px", marginRight: "5px"}}>
                    <div >
                        <Container fixed className="comment-container">
                            {this.state.srcCommentBlock}
                        </Container>
                    </div>

                </DialogContent>
                <DialogAction style={{display:"flex", marginLeft: "20px", marginRight: "20px"}}>
                    <Input id="comment-input" placeholder="友善是交流的起点"  multiline={false} style={{flexGrow: 1}}/>
                    <Button onClick={this.onClickSendComment.bind(this)}>发表</Button>
                    
                </DialogAction>
                <Snackbar  open={this.state.noticeDiaplayState} onClose={this.HideNotice.bind(this)} message={this.state.noticeMsg} autoHideDuration={3000} />
            </Dialog>

        </>
        )
    }

    GenCommentBlock(commentList)
    {
        return commentList.map((comment, index)=>{
            if (comment.re !== "")
            {
                var time = new Date(parseInt(comment.time) * 1000);
                return (
                    <div className="comment-block">
                        <div className="comment-user-info"> 
                            <Avatar src={comment.avatar} />
                            <Link className="comment-post-user" href={"/member/" + comment.username}>{comment.username}</Link>
                            <div className="comment-reply-user">
                                Re: {comment.re}
                            </div>
                        </div>
                        <div className="comment-content">
                            {comment.comment}
                        </div>
                        <div className="comment-footer">
                            <div className="comment-footer-time">{time.toLocaleDateString() + " " + time.toLocaleTimeString('zh-CN', {hour12: false})}</div>
                            {/*<div className="comment-footer-reply">回复</div>}*/}
                        </div>
                    </div>
                )
            }
            else
            {
                var time = new Date(parseInt(comment.time) * 1000);
                return (
                    <div className="comment-block">
                        <div className="comment-user-info"> 
                            <Avatar src={comment.avatar} />
                            <Link className="comment-post-user" href={"/member/" + comment.username}>{comment.username}</Link>
                        </div>
                        <div className="comment-content">
                            {comment.comment}
                        </div>
                        <div className="comment-footer">
                            <div className="comment-footer-time">{time.toLocaleDateString() + " " + time.toLocaleTimeString('zh-CN', {hour12: false})}</div>
                            {/*<div className="comment-footer-reply">回复</div>*/}
                        </div>
                    </div>
                )
            }
        });
    }

    SwitchCommentPage(direction)
    {
        if(direction === "left")
        {
            this.currentCommentPage = this.currentCommentPage - 1
        }
        else if(direction === "right")
        {
            
            this.currentCommentPage = this.currentCommentPage + 1
        }
        else
        {
            this.currentCommentPage = direction;
        }
        this.RetriveComment();
    }

    RetriveComment()
    {
        fetch("/api/v1/content/GetComments?article_id=" + this.articleId + "&page=" + this.currentCommentPage)
        .then(res=>{
            if (res.ok)
            {
                return  res.json();
            }
            else
            {
                this.setState({
                    noticeDiaplayState: true,
                    noticeMsg: "获取评论失败，服务器暂无响应"
                })
                return;
            }
        })
        .then((commentInfo)=>{
            
            this.maxCommentPage = commentInfo.maxPageNum

            if(commentInfo.commentList.length === 0)
            {
                this.setState({
                    srcCommentBlock: <div>暂时还没有评论</div>
                })
                return;
            }

            var pageList = [];
            console.log(commentInfo.maxPageNum);
            console.log(this.currentCommentPage);
            console.log("=================================")
            if (this.maxCommentPage > 4)
            {
                if (commentInfo.maxPageNum - this.currentCommentPage > 2)
                {
                    console.log(commentInfo.maxPageNum);
                    console.log(this.currentCommentPage);
                    for (var index = 0; index <3; index++)
                    {
                        pageList[index] =  this.currentCommentPage + index//[1,2,3,'...', commentInfo.maxPageNum];
                    }
                    pageList.push("---");
                    pageList.push(commentInfo.maxPageNum);
                }
                else
                {
                    pageList[0] = 1;
                    pageList[1] = "---";
                    for (index = 1; index <= 3; index++)
                    {
                        pageList.push(commentInfo.maxPageNum - (3-index));
                    }
                }
            }
            else
            {
                for (var i = 0; i < commentInfo.maxPageNum; ++i)
                {
                    pageList[i] = i+1;
                }
            }

            if (this.currentCommentPage !== 1)
            {
                pageList.push("上一页");
            }
            if (this.currentCommentPage !== this.maxCommentPage )
            {
                pageList.push("下一页");
            }

            this.setState({
                srcCommentBlock: 
                    <>
                        {this.GenCommentBlock(commentInfo.commentList)}
                        <div style={{width: "100%", textAlign: "center"}}>
                            {pageList.map((pageIndex)=>{
                                if(pageIndex === '---')
                                {
                                    return <Button disabled={true}>{pageIndex}</Button>
                                }
                                if(pageIndex === "下一页")
                                {
                                    return <Button onClick={func=>this.SwitchCommentPage("right")}>{pageIndex}</Button>
                                }
                                if(pageIndex === "上一页")
                                {
                                    return <Button onClick={func=>this.SwitchCommentPage("left")}>{pageIndex}</Button>
                                }

                                if(this.currentCommentPage === pageIndex)
                                {
                                    return <Button className="page-button-selected" onClick={func=>this.SwitchCommentPage(pageIndex)}>{pageIndex}</Button>
                                }
                                else
                                {
                                    return <Button className="page-button" onClick={func=>this.SwitchCommentPage(pageIndex)}>{pageIndex}</Button>
                                }
                                
                            })}
                        </div>
                    </>
            })
        })        
    }

    componentDidMount()
    {
        var requestURI = window.location.pathname.match(/\/blogs\/(\d+)/);
        if (requestURI !== null)
        {
            this.articleId = requestURI[1];
        }
        else
        {
            return;
        }

        fetch("/api/v1/content/CheckUserOperation?article_id=" + this.articleId, {credentials: "include"})
        .then(response=>{
            if (response.ok)
            {
                return response.json();
            }
            else
            {
                return;
            }
        })
        .then(userOpertaion=>{
            if (userOpertaion.can_vote !== "1")
            {
                this.voted = true;
                this.setState({
                    voteButtonStyle: {backgroundColor: "#9fd2ea"}
                })
            }
            else
            {
                this.setState({
                    voteButtonStyle: {backgroundColor: "#f5f5f5"}
                })
            }

            if (userOpertaion.can_subscribe !== "1")
            {
                this.subscribed = true;
                this.setState({
                    subscribeButtonStyle: {backgroundColor: "#9fd2ea"}
                })
            }
            else
            {
                this.setState({
                    subscribeButtonStyle: {backgroundColor: "#f5f5f5"}
                })
            }

            if (userOpertaion.isAuthor === "1")
            {
                this.setState({
                    deleteButtonStyle: {color: "red", background: "#f5f5f5"},
                    editButtonStyle:{background: "#f5f5f5"}
                })
            }

        })
    }
}

export default FunctionButtons;