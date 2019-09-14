import React from 'react';
import './App.css';
import Container from "@material-ui/core/Container"
import { Remarkable } from "remarkable"
import hljs from "highlight.js"
import "highlight.js/styles/rainbow.css"

import Navbar from "./components/Navbar"

function ScrollToHashTag()
{
    var target = decodeURI(window.location.hash).substr(1);
    var supportTags = ["h1", "h2", "h3", "h4", "h5"];

    for (var index = 0; index < supportTags.length; index++) {
        var tagList = document.getElementsByTagName(supportTags[index]);
        for (var i = 0; i < tagList.length; i++) {
            if (tagList[i].innerText === target) {
                window.scrollTo(0, tagList[i].offsetTop);
                return;
            }
            else if (tagList[i].innerText === (target.replace(/[-]+?/g, () => { return " " }))) {
                window.scrollTo(0, tagList[i].offsetTop);
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

function UpdateContainer(hookFunc)
{
    var request = new XMLHttpRequest();
    request.onreadystatechange = () => {
        if (request.readyState === 4 && request.status === 200) {
            var article = request.responseText;
            var md = new Remarkable({
                highlight: function (str, lang) {
                    if (lang && hljs.getLanguage(lang)) {
                        var converted = hljs.highlight(lang, str).value;

                        return converted;
                    }
                    else {
                        // prevent xss and incorrect rendering
                        return str.replace(/[<]{1}?/g, '&lt;').replace(/[>]{1}?/g, '&gt;');
                    }
                }
            });

            var converted = md.render(article);
            /* for ```code``` (without language type) */
            converted = converted.replace(/<pre>(.*?)<\/pre>/mgs, (str, g1) => {
                if (g1.match(/.*?<code.*?/) !== null) {
                    return "<pre class='hljs'>" + g1 + "</pre>";
                }
                else {
                    return str;
                }
            });

            /* for `code` */
            converted = converted.replace(/<p>(.*?)<\/p>/mg, (str, g1) => {
                return "<p>" + g1.replace(/<code>/mg, "<code class='hljs unmarked-code'>");
            });

            hookFunc(converted);
            ScrollToHashTag();
        }
    }

    request.open("GET", "/blogs/xman-level1.md", true);
    request.send(null);
}


function App()
{
    const [content, setContent] = React.useState("null");

    UpdateContainer(setContent);
    window.onhashchange = ScrollToHashTag;
    document.body.onclick = ScrollToHashTagClickListener;

    return (
        <>
            <Navbar />
            <Container fixed className="blog-container" dangerouslySetInnerHTML={{ __html: content }} style={{ minWidth: "800px", height: "auto", marginLeft: "auto", marginRight: "auto", background: "#f5f5f5", marginTop: "80px", width: "70%" }} />
        </>
        );
}

export default App;
