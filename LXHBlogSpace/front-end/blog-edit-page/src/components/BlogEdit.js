import React from "react"

import Container from "@material-ui/core/Container"
import Button from "@material-ui/core/Button"
import Input from "@material-ui/core/Input"
import InputLabel from "@material-ui/core/InputLabel"

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
            submitButtonStyle: "tool-bar-button tool-bar-button-submit-ok"
        };
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
            
            //
            fetch("/api/v1/content/UploadImage", {
                method: "POST",
                credentials: "include",
                body: backgroundImageInput.files[0]
            })
            .then(res=>res.json(), (error)=>{})
            .then(response=>{
                if (response !== undefined && response.ecode === "0")
                {
                    document.getElementsByTagName("body")[0].style.backgroundImage = "url(" + response.img_path + ")";
                }
                else
                {

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
                        <Button className="tool-bar-button">上传图片</Button>
                        <Button className="tool-bar-button">保存草稿</Button>
                        <Button className="tool-bar-button tool-bar-button-preview">预览博文</Button>
                        <Button className={this.state.submitButtonStyle}>发表</Button>
                    </div>
                    <InputLabel className="tool-bar-savetime">上次草稿保存的时间：暂未保存</InputLabel>
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
