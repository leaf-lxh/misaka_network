import React from 'react';
import ReactDOM from "react-dom"
import './App.css';
import Container from "@material-ui/core/Container"
import FunctionButtons from "./components/FunctionButtons"

import { Remarkable } from "remarkable"
import hljs from "highlight.js"
import "highlight.js/styles/rainbow.css"
import Link from '@material-ui/core/Link';

class App extends React.Component
{
    constructor(props)
    {
        super(props)
        this.state = {
            renderedBlogContent: <></>
        }
        this.articleId = null;
        this.articleTitle = null;
        this.articleContent = null;
        this.articleAuthor = null;
        this.articleAuthorAvatar = null;
    }

    RenderMarkdown()
    {
        var article = this.articleContent;
        var md = new Remarkable({
            highlight: function (str, lang) {
                if (lang && hljs.getLanguage(lang)) {
                    var converted = hljs.highlight(lang, str).value;

                    return converted;
                }
                else {
                    // prevent xss and incorrect rendering
                    return str.replace(/[<]{1}?/g, '&lt;').replace(/[>]{1}?/g, '&gt;');
                }
            }
        });

        var converted = md.render(article);
        /* for ```code``` (without language type) */
        converted = converted.replace(/<pre>(.*?)<\/pre>/mgs, (str, g1) => {
            if (g1.match(/.*?<code.*?/) !== null) {
                return "<pre class='hljs'>" + g1 + "</pre>";
            }
            else {
                return str;
            }
        });

        /* for `code` */
        converted = converted.replace(/<p>(.*?)<\/p>/mg, (str, g1) => {
            return "<p>" + g1.replace(/<code>/mg, "<code class='hljs unmarked-code'>");
        });

        /* for table border */
        converted = converted.replace(/<table>(.*?)<\/table>/mgs, (str, g1) => {
            return "<table border='1' cellspacing='0'>" +g1 + "</table>" ;
        });

        
        return converted;
    }


    render()
    {
        return (
            <Container fixed className="blog-container" style={{ minWidth: "800px", height: "auto", marginLeft: "auto", marginRight: "auto", background: "#f5f5f5", marginTop: "80px" }} >
                {this.state.renderedBlogContent}   
            </Container>
        )
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
            window.location = "/";
        }

        fetch("/api/v1/content/GetArticleContent?article_id=" + this.articleId, {credentials: 'include'})
        .then(response=>{
            if (response.status === 200)
            {
                return response.json();
            }
            else
            {
                alert("服务器未响应，请稍后再试");
                window.location("/");
            }
        })
        .then(response=>{
            if (response !== undefined && response.ecode === "0")
            {
                this.articleTitle = window.decodeURIComponent(escape(window.atob(response.title)));
                this.articleContent = window.decodeURIComponent(escape(window.atob(response.content)));
                if (response.bg !== "")
                {
                    this.backgroundImage = window.decodeURIComponent(response.bg);
                    var body = document.getElementsByTagName("body")[0];
                    body.style.backgroundImage = "url(http://img-blog.leaflxh.com" + this.backgroundImage + ")";
                    body.style.opacity = 0.85;
                }
                
                this.articleId = response.article_id;

                var time = new Date(parseInt(response.lastmodify_time) * 1000)
                this.setState({
                    renderedBlogContent: 
                    <>
                        <h1>{this.articleTitle}</h1>
                        <div className="article-user-info">
                            作者：<Link href={"/member/" + response.author} underline="none" >{response.author}</Link>
                        </div>
                        <div>发布于：{time.toLocaleDateString() + " " + time.toLocaleTimeString('zh-CN', {hour12: false})}</div>                        
                        <div  dangerouslySetInnerHTML={{ __html: this.RenderMarkdown()}} />
                    </>
                })
                window.blog_info = {"vote_num": response.vote_num, "comment_num": response.comment_num};
                document.title = this.articleTitle + " - LXHBlogSpace";

                ReactDOM.render(<FunctionButtons />, document.getElementById('function-buttons'));
            }
            else
            {
                alert("文章读取失败，原因：" + response.reason);
                window.location = "/"
            }
        })
    }
}

export default App;
