import React from 'react';
// eslint-disable-next-line
import "./css/MainContainer.css"

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
            </div>
        }
        this.lastNode = null;
    }

    render()
    {
        return (
            <>
                {this.state.src}
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
                src: 
                <>
                    <>
                        {this.state.src}
                    </>
                    <>
                    {
                        response.map((blogText, index) =>{
                        var blogData = blogText;
                        this.lastNode = blogData["article_id"];
                        return BlogBriefBox(window.decodeURIComponent(escape(window.atob(blogData["title"]))), window.decodeURIComponent(escape(window.atob(blogData["brief"]))), blogData["tags"], blogData["interInfo"], blogData["authorInfo"], blogData["article_id"]);
                        })
                    }
                    </>
                </>
            });
            document.documentElement.scrollTop = lastPos;
            onloadding = false;
        })
    }

    ScrollHook()
    {
        if ( document.documentElement.scrollHeight - (document.documentElement.scrollTop + document.documentElement.clientHeight) < 500)
        {
            console.log("shit")
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
        fetch("/api/v1/content/GetPublishArticleList")
        .then(response=>response.json())
        .then(response=>{
            console.log(response)
            this.setState({
                src: 
                response.map((blogText, index) =>{
                    var blogData = blogText;
                    this.lastNode = blogData["article_id"];
                    return BlogBriefBox(window.decodeURIComponent(escape(window.atob(blogData["title"]))), window.decodeURIComponent(escape(window.atob(blogData["brief"]))), blogData["tags"], blogData["interInfo"], blogData["authorInfo"], blogData["article_id"]);
                })
            });
            window.onscroll = this.ScrollHook.bind(this);
        })
    }
}

export default BlogBrief;