import React from 'react';
import ReactDOM from 'react-dom';
import './index.css';
import App from './App';

import Navbar from "./components/Navbar"
import * as serviceWorker from './serviceWorker';

function SitckyFuntionButtons()
{
    var container = document.getElementsByClassName("blog-container")[0];
    var panelLeft = container.offsetLeft + container.offsetWidth + 20;
    var panel = document.getElementById("function-buttons");
    panel.style.position = "fixed"
    panel.style.top = "80px";
    panel.style.left= panelLeft + "px";
}

function ScrollToHashTag()
{
    var target = decodeURI(window.location.hash).substr(1);
    var supportTags = ["h1", "h2", "h3", "h4", "h5"];

    for (var index = 0; index < supportTags.length; index++) {
        var tagList = document.getElementsByTagName(supportTags[index]);
        for (var i = 0; i < tagList.length; i++) {
            if (tagList[i].innerText === target) {
                window.scrollTo(0, tagList[i].offsetTop - 64);
                return;
            }
            else if (tagList[i].innerText === (target.replace(/[-]+?/g, () => { return " " }))) {
                window.scrollTo(0, tagList[i].offsetTop - 64);
                return;
            }
        }
    }
}

//绑定为document.body.onclick事件，用于点击同一个a标签的跳转处理
function ScrollToHashTagClickListener(event)
{
    if (event.srcElement.localName === "a") {
        if ((window.location.hash === event.srcElement.hash) && window.location.hash.length > 1) {
            ScrollToHashTag();
        }
    }
}

var requestURI = window.location.pathname.match(/\/blogs\/(\d+)/);
if (requestURI == null)
{
    window.location = "/";
}
else
{
    ReactDOM.render(<Navbar />, document.getElementById('navbar'));
    ReactDOM.render(<App />, document.getElementById('app-main'));
    
    window.onhashchange = ScrollToHashTag;
    document.body.onclick = ScrollToHashTagClickListener;
    
    window.onload = SitckyFuntionButtons;
    window.onresize = SitckyFuntionButtons;
}

// If you want your app to work offline and load faster, you can change
// unregister() to register() below. Note this comes with some pitfalls.
// Learn more about service workers: https://bit.ly/CRA-PWA
serviceWorker.unregister();
