import React from 'react';
// eslint-disable-next-line
import { makeStyles } from '@material-ui/core/styles'

import Container from '@material-ui/core/Container'

import BlogBrief from './BlogBrief'
import MainIndexRightPanel from './MainIndexRightPanel'
import TimeLine from "./TimeLine"
import Water from "./Water"
import CircularProgress from "@material-ui/core/CircularProgress"

import './css/MainContainer.css'


function GenPage(requiredPage) {
    if (requiredPage === "main")
    {
        return (
            <div className="blog-brief-list">
                <BlogBrief />
            </div>
        );
    }
    else if (requiredPage === "timeline")
    {
        return (
            <div className="blog-brief-list">
                <TimeLine />
            </div>
        );
    }
    else if (requiredPage === "water")
    {
        return (
            <div className="blog-brief-list">
                <Water/>
            </div>
        );
    }
    else
    {
        return (
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
        );
    }
}

function StickyRightPanel(){
    var briefList = document.getElementsByClassName("blog-brief-list")[0];
    var panelLeft = briefList.offsetLeft + briefList.offsetWidth + 20;
    var panel = document.getElementsByClassName("index-right-panel")[0];
    panel.style.position = "fixed"
    panel.style.top = "80px";
    panel.style.left= panelLeft + "px";
}

const MainContainer = (requiredPage) => {
    //设置右边面板悬浮
    window.onload = StickyRightPanel;
    window.onresize = StickyRightPanel;

    return(
        <Container fixed className="main-container">
            {GenPage(requiredPage)}
            <div  className="index-right-panel">
                <MainIndexRightPanel/>
            </div>
            
        </Container>
    )
}

export default MainContainer;