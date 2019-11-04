import React from 'react';
// eslint-disable-next-line
import "./css/MainContainer.css"
import "./css/BlogBrief.css"

import BlogBriefBox from './BlogBriefBox'
import CircularProgress from "@material-ui/core/CircularProgress"

var onloadding = false;
var lastPos = null;
class BlogBrief extends React.Component
{
    constructor(props)
    {
        super(props);
        this.state = {
            src: 
            <div className="blog-brief-list">
                <div className="container-loading">
                    <div style={{paddingTop: "300px"}}>
                        <CircularProgress style={{display: "inline-block"}} variant="indeterminate"/>
                        <div className="container-loading-text">
                            加载中...
                        </div>
                    </div>

                </div>
            </div>,
            articleList: this.props.articleList
        }
        this.lastNode = null;
    }

    render()
    {
        if (this.props.articleList.length === 0)
        {
            return (
                <div className="nothing-onthe-line">
                        <div style={{paddingTop: "300px"}}>
                            <div className="nothing-onthe-line-text">
                                暂时没有文章，快去创建第一篇文章吧=>
                            </div>
                        </div>
                </div>
            )
        }

        return (
            <>
                {this.state.articleList.map((blogText, index)=>{
                    var blogData = blogText;
                    this.lastNode = blogData["article_id"];
                    return BlogBriefBox(window.decodeURIComponent(escape(window.atob(blogData["title"]))), window.decodeURIComponent(escape(window.atob(blogData["brief"]))), blogData["tags"], blogData["interInfo"], blogData["authorInfo"], blogData["article_id"], blogData["create_date"]);
                })}
            </>
        )
    }

    LoadMoreArticle()
    {
        fetch("/api/v1/content/GetPublishArticleList?article_id=" + this.lastNode)
        .then(response=>response.json())
        .then(response=>{
            if (response.length === 0)
            {
                window.onscroll = null;
            }
            
            this.setState({
                articleList: this.state.articleList.concat(response)
            });
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
        document.documentElement.scrollTop = 0;
        window.onscroll = this.ScrollHook.bind(this);
        document.addEventListener("unload-brief-hook", ()=>{
            window.onscroll = null;
        })

        document.addEventListener("load-brief-hook", ()=>{
            window.onscroll = this.ScrollHook.bind(this);
        })
    }
}

export default BlogBrief;