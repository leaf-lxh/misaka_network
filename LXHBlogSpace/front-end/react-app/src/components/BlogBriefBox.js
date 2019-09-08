import React from 'react';
import Avatar from '@material-ui/core/Avatar'
import Chip from '@material-ui/core/Chip'
import "./css/BlogBriefBox.css"

import AccessTime from "@material-ui/icons/AccessTime"

function GetTimeBlock(time)
{
    if (time != null)
    {
        return (
            <div className="blog-brief-timestamp">
                <div className="blog-brief-timestamp-iconitem"><AccessTime /></div>
                <div className="blog-brief-timestamp-timeitem">{time}</div>
            </div>
        )
    }
    else{
        return "";
    }
}
function BlogBriefBox(title, article, tags, interInfo, userInfo, href, time)  {
    return(
        <div className="blog-brief-root">
            {GetTimeBlock(time)}
            <div className="blog-brief-head">
                <div className="title">{title}</div>
                <div className="user-info">
                    <div className="username">{userInfo["name"]}</div>
                    <Avatar className="avatar" src={userInfo["avatar"]} style={{borderRadius: 0}}/>
                </div>
            </div>

            <div className="blog-brief-content-block">
                <span className="brief-content">{article}</span>
                <button className="brief-moto" onClick={()=>{window.open(href)}}>阅读全文</button>
            </div>

            <div className="blog-brief-footer">
                <div className="blog-brief-tag-list">
                    {
                        tags.map((tag, index) =>{
                            return <Chip className="tag-item" label={tag} style={{borderRadius:3}}/>
                        })
                    }
                </div>
                <div className="blog-inter-info">
                    <Chip className="info-vote" style={{borderRadius: 0}} label={"赞同 " + interInfo["vote"]} />
                    <Chip className="info-comments" style={{borderRadius: 0}} label={interInfo["comments"] + " 条评论" } />
                </div>
            </div>
        </div>
    )
}

export default BlogBriefBox;