import React from 'react';
// eslint-disable-next-line
import { makeStyles } from '@material-ui/core/styles'

import Container from '@material-ui/core/Container'

import BlogBrief from './BlogBrief'
import MainIndexRightPanel from './MainIndexRightPanel'

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
                timeline.
            </div>
        );
    }
    else
    {
        return (
            <div className="blog-brief-list">
                water
            </div>
        );
    }
}

function AdjustRightPanelPos(){
    var briefList = document.getElementsByClassName("blog-brief-list")[0];
    var panelLeft = briefList.offsetLeft + briefList.offsetWidth + 20;
    var panel = document.getElementsByClassName("index-right-panel")[0];
    panel.style.position = "fixed"
    panel.style.top = "80px";
    panel.style.left= panelLeft + "px";
}

const MainContainer = (requiredPage) => {
    //设置右边面板悬浮
    window.onload = AdjustRightPanelPos;
    window.onresize = AdjustRightPanelPos;

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