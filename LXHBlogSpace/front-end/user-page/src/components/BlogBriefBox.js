import React from 'react';
import Avatar from '@material-ui/core/Avatar'
import Chip from '@material-ui/core/Chip'
import "./css/BlogBriefBox.css"

import AccessTime from "@material-ui/icons/AccessTime"

function GetTimeBlock(time)
{
    if (time != null)
    {
        time = new Date(parseInt(time) * 1000);
        return (
            <div className="blog-brief-timestamp">
                <div className="blog-brief-timestamp-iconitem"><AccessTime /></div>
                <div className="blog-brief-timestamp-timeitem">{time.toLocaleDateString() + " " + time.toLocaleTimeString('zh-CN', {hour12: false})}</div>
            </div>
        )
    }
    else{
        return "";
    }
}
function BlogBriefBox(title, article, tags, interInfo, userInfo, href, time)  {
    time = new Date(parseInt(time) * 1000);
    return(
        <div className="blog-brief-root">
            <div className="blog-brief-head">
                <div className="title">{title}</div>
            </div>

            <div className="blog-brief-content-block">
                <span className="brief-content">{article}</span>
                <button className="brief-moto" onClick={()=>{window.open("/blogs/" + href)}}>阅读全文</button>
            </div>

            <div className="blog-brief-footer">
                <div className="blog-brief-tag-list">
                发表于：{time.toLocaleDateString() + " " + time.toLocaleTimeString('zh-CN', {hour12: false})}
                </div>
                <div className="blog-inter-info">
                    <Chip className="info-vote" label={"赞同 " + interInfo["vote"]} />
                    <Chip className="info-comments" label={interInfo["comments"] + " 条评论" } />
                </div>
            </div>
        </div>
    )
}

export default BlogBriefBox;