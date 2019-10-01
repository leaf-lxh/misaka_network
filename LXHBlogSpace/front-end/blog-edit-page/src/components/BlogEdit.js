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
            previewDialogLink: ""
        };

        this.uploadedImages = [];
    }

    PreviewImage(index)
    {
        this.setState({
            previewDialogDisplay: true,
            previewDialogTitle: this.uploadedImages[index].name,
            previewDialogLink: this.uploadedImages[index].href
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
                    this.uploadedImages.push({"name": file.name, "href": "http://img-blog.leaflxh.com/" + response.img_path});
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

    UploadBackground()
    {

        var backgroundImageInput = document.getElementById("input-upload-background");
        console.log(backgroundImageInput);
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
            .then(response=>{
                if (response === undefined)
                {    
                    return;
                }

                if (response.ecode === "0")
                {
                    document.getElementsByTagName("body")[0].style.backgroundImage = "url(http://img-blog.leaflxh.com" + response.img_path + ")";
                }
                else
                {
                    alert("上传失败，原因：" + response.reason);
                }
                return;
            })
        }
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
                        <Button className="tool-bar-button"  onClick={this.UploadImage.bind(this)}>上传图片</Button>
                        <Button className="tool-bar-button">保存草稿</Button>
                        <Button className="tool-bar-button tool-bar-button-preview">预览博文</Button>
                        <Button className={this.state.submitButtonStyle}>发表</Button>
                    </div>
                    <InputLabel className="tool-bar-savetime">上次草稿保存的时间：暂未保存</InputLabel>
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
                <div className="article-title">
                    <Input placeholder="在此输入文章标题"  className="article-title-input"></Input>
                </div>
                <Container fixed className="article-content-container">
                    <div className="flexable-container">
                        <Input id="article-content-input" className="article-content-region" disableUnderline placeholder="在此输入文章内容" multiline rows={25} rowsMax={1000}></Input>
                    </div>
                </Container>
            </div>
        )
    }
}

export default BlogEdit;
