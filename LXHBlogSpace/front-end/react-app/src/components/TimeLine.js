import BlogBriefBox from "./BlogBriefBox"
import React from "react"
import "./css/TimeLine.css"

var onloadding = false;
var lastPos;

class TimeLine extends React.Component
{
    constructor(props)
    {
        super(props);
        this.lastLoadInfo = {
            time: null,
            index: null
        }
        this.state = {
            articleList: this.props.articleList
        }
    }

    RenderBlogBreifBox(article)
    {
        this.lastLoadInfo.time = article.create_date;
        this.lastLoadInfo.index = article.index;

        var customTime = "";
        var currentTime = Math.round((new Date()).getTime() / 1000);
        if (currentTime - article.create_date < 60)
        {
            customTime = Math.round(currentTime - article.create_date) + "秒前";
        }
        else if (currentTime - article.create_date < 60*60)
        {
            customTime = Math.round((currentTime - article.create_date) / 60) + "分钟前";
        }
        else if (currentTime - article.create_date < 60*60*24)
        {
            customTime = Math.round((currentTime - article.create_date) / 60 / 60) + "小时前";
        }
        else if (currentTime - article.create_date < 60*60*24*365)
        {
            customTime = Math.round((currentTime - article.create_date) / 60 / 60 / 24) + "天前";
        }
        else
        {
            customTime = Math.round((currentTime - article.create_date) / 60 / 60 / 24 / 365) + "年前";
        }
        
        return BlogBriefBox(window.decodeURIComponent(escape(window.atob(article.title))), window.decodeURIComponent(escape(window.atob(article.brief))), null, article.interInfo, article.authorInfo, article.article_id, article.create_date, customTime)
    }

    render()
    {
        if (this.props.articleList === undefined)
        {
            return (
                <div className="nothing-onthe-line">
                        <div style={{paddingTop: "300px"}}>
                            <div className="nothing-onthe-line-text">
                                服务器未响应，请稍后再试
                            </div>
                        </div>
                </div>
            )
        }
        if (this.props.articleList.length === 0)
        {
            return (
                <div className="nothing-onthe-line">
                        <div style={{paddingTop: "300px"}}>
                            <div className="nothing-onthe-line-text">
                                时间线上暂无文章，快去关注几个用户吧~
                            </div>
                        </div>
                </div>
            )
        }
        else
        {
            return (
                <>
                    {this.state.articleList.map((article, index)=>{return this.RenderBlogBreifBox(article, index)})}
                </>
            )
        }
    }

    LoadMoreArticle()
    {
        fetch("/api/v1/content/GetFollowedArticleList?time=" + this.lastLoadInfo.time + "&index=" + this.lastLoadInfo.index, {"credentials": "include"})
        .then(res=>res.json())
        .then(list=>{
            if (list.length === 0)
            {
                window.onscroll = null;
            }
            else
            {
                this.setState({
                    articleList: this.state.articleList.concat(list)
                })
            }
            onloadding = false;
        })
    }

    ScrollHook()
    {
        if ( document.documentElement.scrollHeight - (document.documentElement.scrollTop + document.documentElement.clientHeight) < 500)
        {
            if (onloadding === false)
            {
                onloadding = true;
                lastPos = document.documentElement.scrollTop;
                this.LoadMoreArticle()
            }

        }
    }

    componentDidMount()
    {
        //mouse event
        document.documentElement.scrollTop = 0;
        window.onscroll = this.ScrollHook.bind(this)
        document.addEventListener("unload-timeline-hook", ()=>{
            window.onscroll = null;
        })

    }



}
export default TimeLine;