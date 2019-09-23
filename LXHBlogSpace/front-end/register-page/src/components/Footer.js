import React from "react"

import "./css/Footer.css"
class Footer extends React.Component
{
    render()
    {
        return (
            <div className="site-footer">
                <div>
                    Powered by React, material-ui, and lxh's tinyHttpd. 
                </div>
                <div>
                    ©2019&nbsp;&nbsp;LXHBlogSapce
                </div>
                &nbsp;&nbsp;
            </div>
        )
    }
}

export default Footer;