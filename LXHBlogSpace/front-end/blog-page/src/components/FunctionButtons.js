import React from 'react'
import Button from "@material-ui/core/Button"
import PlusOne from "@material-ui/icons/PlusOne"
import AddBox from "@material-ui/icons/AddBox"
import Message from "@material-ui/icons/Message"

import Dialog from "@material-ui/core/Dialog"
import DialogTitle from "@material-ui/core/DialogTitle"
import DialogAction from "@material-ui/core/DialogActions"
import DialogContent from "@material-ui/core/DialogContent"

import Input from "@material-ui/core/Input"

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
            voteNum: 0,
            commentNum: 0,
            dialogDisplayState: false,
            noticeDiaplayState: false,
            noticeMsg: "null",
            srcCommentBlock: <></>
        }
        this.currentCommentPage = 1;
        this.maxCommentPage = 1;
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
        this.setState({
            noticeDiaplayState: true,
            noticeMsg: "发表成功"
        })
    }

    HideNotice()
    {
        this.setState({
            noticeDiaplayState: false
        })
    }

    render()
    {
        return (
        <>
            <Button className="func-button">
                <PlusOne />
                <div className="button-label">
                    {this.state.voteNum}&nbsp;赞同
                </div>
            </Button>
            <Button className="func-button" onClick={this.ShowDialog.bind(this)}>
                <Message />
                <div className="button-label">
                    {this.state.commentNum}&nbsp;评论
                </div>
            </Button>
            <Button className="func-button">
                <AddBox />
                <div className="button-label">
                添加收藏
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
                    <Input placeholder="友善是交流的起点"  multiline={false} style={{flexGrow: 1}}/>
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
                return (
                    <div className="comment-block">
                        <div className="comment-user-info"> 
                            <Avatar src={comment.avatar} />
                            <div className="comment-post-user">
                                {comment.username}  
                            </div>
                            <div className="comment-reply-user">
                                Re: {comment.re}
                            </div>
                        </div>
                        <div className="comment-content">
                            {comment.comment}
                        </div>
                        <div className="comment-footer">
                            <div className="comment-footer-time">{comment.time}</div>
                            <div className="comment-footer-reply">回复</div>
                        </div>
                    </div>
                )
            }
            else
            {
                return (
                    <div className="comment-block">
                        <div className="comment-user-info"> 
                            <Avatar src={comment.avatar} />
                            <div className="comment-post-user">
                                {comment.username}  
                            </div>
                        </div>
                        <div className="comment-content">
                            {comment.comment}
                        </div>
                        <div className="comment-footer">
                            <div className="comment-footer-time">{comment.time}</div>
                            <div className="comment-footer-reply">回复</div>
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
        fetch("/api/v1/GetComments?page=" + this.currentCommentPage).then(res=> res.json()).then((commentInfo)=>{
            this.maxCommentPage = commentInfo.maxPageNum

            if(commentInfo.commentList === [])
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
}

export default FunctionButtons;