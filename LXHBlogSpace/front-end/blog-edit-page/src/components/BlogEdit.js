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

    render()
    {
        return (
            <div className="edit-container">
                <div className="tool-bar">
                    <div className="tool-bar-buttons">
                        <Input className="tool-bar-button" type="file" style={{display: "none"}}>上传背景</Input>
                        <Input className="tool-bar-button" type="file" style={{display: "none"}}>上传图片</Input>
                        <Button className="tool-bar-button">上传背景</Button>
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
                        <Input id="article-content-input" className="article-content-region" disableUnderline placeholder="在此输入文章内容" multiline></Input>
                    </div>
                </Container>
            </div>
        )
    }
}

export default BlogEdit;