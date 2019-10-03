import React from "react"
import ReactDOM from "react-dom"

import Container from "@material-ui/core/Container"
import Button from "@material-ui/core/Button"
import ButtonBase from "@material-ui/core/ButtonBase"
import Input from "@material-ui/core/Input"
import InputLabel from "@material-ui/core/InputLabel"

import Card from "@material-ui/core/CardMedia"
import CardMedia from "@material-ui/core/CardMedia"
import Dialog from "@material-ui/core/Dialog"
import DialogTitle from '@material-ui/core/DialogTitle';

import Delete from "@material-ui/icons/Delete"

import { Remarkable } from "remarkable"
import hljs from "highlight.js"
import "highlight.js/styles/rainbow.css"

import "./css/BlogEdit.css"


class BlogEdit extends React.Component
{
    constructor(props)
    {
        super(props);
        this.state = {
            rawBlogContent: "",
            previewContent: false,
            srcEditRegion: <></>,
            submitButtonStyle: "tool-bar-button tool-bar-button-submit-ok",
            previewDialogDisplay: false,
            previewDialogTitle: "",
            previewDialogLink: "",
            removeBackgroundButtonDisable: true,
            saveTime: "暂未保存",
            saveDisabled: false,
            enablePreviewMode: false,
            previewButtonLabel: "预览博文",
            renderedBlogContent: <></>
        };

        this.articleId = null;
        this.draftId = null;
        this.uploadedImages = [];
        this.backgroundImage = "";
    }

    PreviewImage(index)
    {
        this.setState({
            previewDialogDisplay: true,
            previewDialogTitle: this.uploadedImages[index].name,
            previewDialogLink: "http://img-blog.leaflxh.com/" + this.uploadedImages[index].href
        });
    }

    ClosePreview()
    {
        this.setState({
            previewDialogDisplay: false,
        });
    }

    DeleteImage(index)
    {
        this.uploadedImages.splice(index, 1);
        this.RenderImagesZone();
    }

    RenderImagesZone()
    {
        ReactDOM.render (
            <>
                {
                    this.uploadedImages.map((property, index)=>{
                        return (
                            <div className="image-link-bar">
                                <ButtonBase className="image-delete" title="删除" size="small" disableRipple={true} onClick={this.DeleteImage.bind(this, index)}><Delete color="error"/></ButtonBase>
                                <Button className="image-url-preview" title="点击预览" disableRipple={true} onClick={this.PreviewImage.bind(this, index)}>{property.name}</Button>
                            </div>
                        )
                    })
                }
            </>
        , document.getElementsByClassName("uploaded-images")[0])
    }

    UploadImage()
    {
        var ImageInput = document.getElementById("input-upload-img");
        ImageInput.click();
        ImageInput.onchange = function(){
            var file = ImageInput.files[0];
            if (file.type !== "image/png" && file.type !== "image/jpeg" && file.type !== "image/bmp")
            {
                alert("仅允许使用png, jpeg或bmp格式的图片");
                return;
            }
            
            //
            fetch("/api/v1/content/UploadImage", {
                method: "POST",
                credentials: "include",
                body: ImageInput.files[0]
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
            .then(response=>{
                if (response === undefined)
                {
                    return;
                }

                if (response.ecode === "0")
                {
                    this.uploadedImages.push({"name": file.name, "href": response.img_path});
                    ImageInput.value = null;
                    this.RenderImagesZone();
                }
                else
                {
                    alert("上传失败，原因：" + response.reason);
                }
                return;
            })
        }.bind(this);
    }

    SaveDraft()
    {

        var draftid;
        
        if (this.draftId === null)
        {
            draftid = "";
        }
        else
        {
            draftid = this.draftId;
        }

        var backgroundPath = this.backgroundImage;
        var title = document.getElementById("article-title-input").value;
        var content = document.getElementById("article-content-input").value

        fetch("/api/v1/content/SaveDraft", {
            method: "POST",
            credentials: "include",
            headers: {
                "Content-Type": "application/x-www-form/urlencoded"
            },
            body: "draft_id=" + draftid + "&bg=" + window.encodeURIComponent(backgroundPath) + "&imgs=" + window.encodeURIComponent(JSON.stringify(this.uploadedImages)) + "&title=" + window.btoa(unescape(window.encodeURIComponent(title))) + "&content=" + window.btoa(unescape(window.encodeURIComponent(content)))
        })
        .then(response=>{
            if (response.ok !== true)
            {
                alert("存储至服务器失败，建议您手工保存至电脑");
                return;
            }

            return response.json();
        })
        .then(response=>{
            if (response !== undefined && response.ecode === "0")
            {
                var time = new Date();
                this.setState({
                    saveTime: time.toLocaleDateString() + " " + time.toLocaleTimeString('zh-CN', {hour12: false})
                });
                this.draftId = response.draft_id;
            }
            else
            {
                alert("保存失败，原因：" + response.reason);
            }
        })

    }

    UploadBackground()
    {
        var backgroundImageInput = document.getElementById("input-upload-background");
        backgroundImageInput.click();
        backgroundImageInput.onchange = function(){
            var file = backgroundImageInput.files[0];
            if (file.type !== "image/png" && file.type !== "image/jpeg" && file.type !== "image/bmp")
            {
                alert("仅允许使用png, jpeg或bmp格式的图片");
                return;
            }
            
            fetch("/api/v1/content/UploadImage", {
                method: "POST",
                credentials: "include",
                body: backgroundImageInput.files[0]
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
                    document.getElementsByTagName("body")[0].style.backgroundImage = "url(http://img-blog.leaflxh.com" + response.img_path + ")";
                    this.backgroundImage = response.img_path;
                    this.setState({
                        removeBackgroundButtonDisable: false
                    })
                    backgroundImageInput.value = null;
                }
                else
                {
                    alert("上传失败，原因：" + response.reason);
                }
                return;
            }.bind(this))
        }.bind(this)
    }

    RemoveBackground()
    {
        document.getElementsByTagName("body")[0].style.backgroundImage = null;
        this.backgroundImage = "";
        this.setState({
            removeBackgroundButtonDisable: true
        })
    }

    RenderMarkdown()
    {
        var article = document.getElementById("article-content-input").value;
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

    PreviewArticle()
    {
        if (this.state.enablePreviewMode === true)
        {
            this.setState({
                enablePreviewMode: false,
                previewButtonLabel: "预览博文"
            })
            return;
        }
        
        var time = new Date();
        this.setState({
            enablePreviewMode: true,
            previewButtonLabel: "返回编辑",
            renderedBlogContent: 
                <>
                    <h1>{document.getElementById("article-title-input").value}</h1>
                    <div>创建于：{time.toLocaleDateString() + " " + time.toLocaleTimeString('zh-CN', {hour12: false})}</div>
                    <div  dangerouslySetInnerHTML={{ __html: this.RenderMarkdown()}} />
                </>
        })
    }

    PublishArticle()
    {
        var articleid;
        if (this.articleId === null)
        {
            articleid = "";
        }
        else
        {
            articleid = this.articleId;
        }

        var backgroundPath = this.backgroundImage;
        var title = document.getElementById("article-title-input").value;
        var content = document.getElementById("article-content-input").value

        fetch("/api/v1/content/PublishArticle", {
            method: "POST",
            credentials: "include",
            headers: {
                "Content-Type": "application/x-www-form/urlencoded"
            },
            body: "article_id=" + articleid + "&bg=" + window.encodeURIComponent(backgroundPath) + "&title=" + window.btoa(unescape(window.encodeURIComponent(title))) + "&content=" + window.btoa(unescape(window.encodeURIComponent(content)))
        })
        .then(response=>{
            if (response.ok !== true)
            {
                alert("发布文章失败");
                return;
            }

            return response.json();
        })
        .then(response=>{
            if (response !== undefined && response.ecode === "0")
            {
                window.location = "/blogs/" + response.article_id
            }
            else
            {
                alert("发布失败，原因：" + response.reason);
            }
        })
    }

    render()
    {
        return (
            <div className="edit-container">
                <div className="tool-bar">
                    <div className="tool-bar-buttons">
                        <Input id="input-upload-background" type="file" style={{display: "none"}}>上传背景</Input>
                        <Input id="input-upload-img" type="file" style={{display: "none"}}>上传图片</Input>
                        <Button className="tool-bar-button" onClick={this.UploadBackground.bind(this)}>上传背景</Button>
                        <Button className="tool-bar-button" disabled={this.state.removeBackgroundButtonDisable} onClick={this.RemoveBackground.bind(this)}>删除背景</Button>
                        <Button className="tool-bar-button"  onClick={this.UploadImage.bind(this)}>上传图片</Button>
                        <Button className="tool-bar-button" onClick={this.SaveDraft.bind(this)}>保存草稿</Button>
                        <Button className="tool-bar-button tool-bar-button-preview" onClick={this.PreviewArticle.bind(this)}>{this.state.previewButtonLabel}</Button>
                        <Button className={this.state.submitButtonStyle} onClick={this.PublishArticle.bind(this)}>发表</Button>
                    </div>
                    <InputLabel className="tool-bar-savetime">上次草稿保存的时间：{this.state.saveTime}</InputLabel>
                    <div className="uploaded-images" /> 
                    <Dialog onClose={this.ClosePreview.bind(this)} open={this.state.previewDialogDisplay} >
                        <DialogTitle>
                            {this.state.previewDialogTitle}
                        </DialogTitle>
                        <Card>
                                <CardMedia component="img" image={this.state.previewDialogLink}/>
                                <Input readOnly={true} disabled={true} value={this.state.previewDialogLink} fullWidth={true}/>
                        </Card>
                    </Dialog>
                </div>
                <div className="article-edit-region" hidden={this.state.enablePreviewMode}>
                    <div className="article-title" >
                        <Input id="article-title-input" placeholder="在此输入文章标题"  className="article-title-region"></Input>
                    </div>
                    <Container fixed className="article-content-edit-container">
                        <div className="flexable-container">
                            <Input id="article-content-input" className="article-content-region" disableUnderline placeholder="在此输入文章内容" multiline rows={25} rowsMax={1000}></Input>
                        </div>
                    </Container>
                </div>
                <div className="article-preview-region" hidden={!this.state.enablePreviewMode}>
                    <Container fixed className="article-content-container"  style={{ minWidth: "800px", height: "auto", marginLeft: "auto", marginRight: "auto", display: "block"}} >
                        {this.state.renderedBlogContent}
                    </Container>
                </div>
            </div>
        )
    }

    componentDidMount()
    {
        var requestParam = window.location.search.match(/\?(\w+)=(\d+)/);
        if (requestParam === null)
        {
            return;
        }

        if (requestParam[1] === "draft")
        {
            this.draftId = requestParam[2];
            console.log(this);
            fetch("/api/v1/content/ReadDraft?draft_id=" + this.draftId, {
                credentials: 'include'
            })
            .then(response=>{
                if (response.status === 200)
                {
                    return response.json();
                }
                else if (response.status === 403)
                {
                    alert("您没有访问此草稿的权限");
                    window.location = "/edit"
                    return;
                }
                else
                {
                    alert("服务器未响应，请稍后再试");
                    window.location = "/edit"
                    return;
                }
            })
            .then(response=>{
                if (response !== undefined && response.ecode === "0")
                {
                    document.getElementById("article-title-input").value= window.decodeURIComponent(escape(window.atob(response.title)));
                    document.getElementById("article-content-input").value = window.decodeURIComponent(escape(window.atob(response.content)));
                    if (response.bg !== "")
                    {
                        this.backgroundImage = window.decodeURIComponent(response.bg);
                        document.getElementsByTagName("body")[0].style.backgroundImage = "url(http://img-blog.leaflxh.com" + this.backgroundImage + ")";
                        this.setState({
                            removeBackgroundButtonDisable: false
                        })
                    }
                    var time = new Date(parseInt(response.lastmodify_time) * 1000)
                    this.setState({
                        saveTime: time.toLocaleDateString() + " " + time.toLocaleTimeString('zh-CN', {hour12: false})
                    })
                    this.uploadedImages = JSON.parse(window.decodeURIComponent(response.imgs));
                    this.RenderImagesZone();
                    
                }
                else
                {
                    alert("文章读取失败，原因：" + response.reason);
                    window.location = "/edit"
                }
            })
        }
        else if (requestParam[1] === "article")
        {
            fetch("/api/v1/content/GetArticleContent?article_id=" + requestParam[2], {
                credentials: 'include'
            })
            .then(response=>{
                if (response.status === 200)
                {
                    return response.json();
                }
                else if (response.status === 403)
                {
                    alert("您没有编辑此文章的权限");
                    window.location = "/edit"
                    return;
                }
                else
                {
                    alert("服务器未响应，请稍后再试");
                    window.location = "/edit"
                    return;
                }
            })
            .then(response=>{
                if (response !== undefined && response.ecode === "0")
                {
                    document.getElementById("article-title-input").value= window.decodeURIComponent(escape(window.atob(response.title)));
                    document.getElementById("article-content-input").value = window.decodeURIComponent(escape(window.atob(response.content)));
                    if (response.bg !== "")
                    {
                        this.backgroundImage = window.decodeURIComponent(response.bg);
                        document.getElementsByTagName("body")[0].style.backgroundImage = "url(http://img-blog.leaflxh.com" + this.backgroundImage + ")";
                        this.setState({
                            removeBackgroundButtonDisable: false
                        })
                    }
                    this.RenderImagesZone();
                    
                    this.articleId = response.article_id;
                }
                else
                {
                    alert("文章读取失败，原因：" + response.reason);
                    window.location = "/edit"
                }
            })
        }
    }
}

export default BlogEdit;
