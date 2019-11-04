import React from 'react';
import ReactDOM from 'react-dom';
// eslint-disable-next-line
import { makeStyles } from '@material-ui/core/styles'

import Container from '@material-ui/core/Container'

import BlogBrief from './BlogBrief'
import MainIndexRightPanel from './MainIndexRightPanel'
import TimeLine from "./TimeLine"
import Water from "./Water"
import CircularProgress from "@material-ui/core/CircularProgress"

import './css/MainContainer.css'

function StickyRightPanel(){
    var briefList = document.getElementsByClassName("blog-brief-list")[0];
    var panelLeft = briefList.offsetLeft + briefList.offsetWidth + 20;
    var panel = document.getElementsByClassName("index-right-panel")[0];
    panel.style.position = "fixed"
    panel.style.top = "80px";
    panel.style.left= panelLeft + "px";
}

class MainContainer extends React.Component
{
    //设置右边面板悬浮
    constructor(props)
    {
        super(props)
        window.onload = StickyRightPanel;
        window.onresize = StickyRightPanel;
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
        document.addEventListener("container-switch", function(msg){
            this.SwitchPage(msg.detail)
        }.bind(this))
    }

    render()
    {
        return(
            <Container fixed className="main-container">
                {this.state.src}
                <div  className="index-right-panel">
                    <MainIndexRightPanel/>
                </div>
                
            </Container>
        )
    }

    componentDidMount()
    {
        this.SwitchPage(this.props.page)
    }


    SwitchPage(requiredPage) 
    {
        if (requiredPage === "main")
        {
            var event = new Event("unload-timeline-hook");
            document.dispatchEvent(event);
            fetch("/api/v1/content/GetPublishArticleList")
            .then(response=>response.json())
            .then(response=>{
                this.setState({
                    src:
                    <div className="blog-brief-list">
                        <BlogBrief articleList={response}/>
                    </div>
                })
            })
        }
        else if (requiredPage === "timeline")
        {
            var event = new Event("unload-brief-hook");
            document.dispatchEvent(event);
            fetch("/api/v1/content/GetFollowedArticleList", {'credentials': 'include'})
            .then(res=>{
                if (res.ok)
                {
                    return res.json()
                }
            })
            .then(function(res)
            {
                this.setState({
                    src:
                    <div className="blog-brief-list">
                        <TimeLine articleList={res}/>
                    </div>
                })
            }.bind(this))
        }
        else if (requiredPage === "water")
        {
            this.setState({
                src:
                <div className="blog-brief-list">
                    <Water/>
                </div>
            });
        }
        else
        {
            this.setState({
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
            });
        }
    }
}

export default MainContainer;