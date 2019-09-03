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

const MainContainer = (requiredPage) => {
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