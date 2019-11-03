import BlogBriefBox from "./BlogBriefBox"
import React from "react"

class TimeLine extends React.Component
{
    constructor(props)
    {
        super(props);
        this.lastLoadInfo = {
            time: null,
            limit: null
        }
        this.state = {
            articleList: this.props.articleList
        }
    }

    RenderBlogBreifBox(article)
    {
        return BlogBriefBox(window.decodeURIComponent(escape(window.atob(article.title))), window.decodeURIComponent(escape(window.atob(article.brief))), null, article.interInfo, article.authorInfo, "/blogs/"+article.article_id, article.create_date)
    }

    render()
    {

        if (this.props.articleList === undefined)
        {
            return (
                <div className="nothing-onthe-line">
                    服务器未响应，请稍后再试
                </div>
            )
        }
        if (this.props.articleList.length === 0)
        {
            return (
                <div className="nothing-onthe-line">
                    未获取到时间线上的文章列表
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

    componentDidMount()
    {
        //mouse event
    }



}
export default TimeLine;