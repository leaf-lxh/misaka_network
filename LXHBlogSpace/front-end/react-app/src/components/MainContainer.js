import React from 'react';
// eslint-disable-next-line
import { makeStyles } from '@material-ui/core/styles'

import Container from '@material-ui/core/Container'

import BlogBrief from './BlogBrief'
import UserPanel from './UserPanel'

import './css/MainContainer.css'

const MainContainer = () => {
    return(
        <Container fixed className="main-container">
            <div className="blog-brief-list">
                <BlogBrief />
            </div>
            
            <div>
                <UserPanel />
            </div>
            
        </Container>
    )
}

export default MainContainer;