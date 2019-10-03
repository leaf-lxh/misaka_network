import React from 'react';
import ReactDOM from 'react-dom';
import './index.css';
import * as serviceWorker from './serviceWorker';

import Navbar from "./components/Navbar"
import BlogEdit from "./components/BlogEdit"

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

window.onhashchange = ScrollToHashTag;
document.body.onclick = ScrollToHashTagClickListener;

ReactDOM.render(<Navbar />, document.getElementById('navbar'));
ReactDOM.render(<BlogEdit />, document.getElementById('article-edit-region'));
serviceWorker.unregister();

