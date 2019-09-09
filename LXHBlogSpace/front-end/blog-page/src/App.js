import React from 'react';
import './App.css';
import Container from "@material-ui/core/Container"
import {Remarkable} from "remarkable"
import hljs from "highlight.js"
import "highlight.js/styles/rainbow.css"

function UpdateContainer(hookFunc)
{
  var request = new XMLHttpRequest();
  request.onreadystatechange = () =>{
    if (request.readyState === 4 && request.status === 200)
    {
      var article = request.responseText;
      var md = new Remarkable({highlight : function(str, lang){
        if (lang && hljs.getLanguage(lang))
        {
          var converted = hljs.highlight(lang, str).value;
          return converted;
        }
        else
        {
          return str;
        }
      }});

      var converted = md.render(article);
      /* for ```code``` (without language type) */
      converted = converted.replace(/<pre>(.*?)pre>/mgs, (str, g1)=>{
        console.log(g1);
        if (g1.match(/.*?<code.*?/) !== null)
        {
          return "<pre class='hljs'>" + g1 + "pre>";
        }
        else
        {
          return str;
        }
      });

      /* for `code` */
      converted = converted.replace(/<p>(.*?)<\/p>/mg, (str, g1)=>{
        return "<p>" + g1.replace(/<code>/mg, "<code class='hljs unmarked-code'>");
      });

      hookFunc(converted);
    }
  }

  request.open("GET", "/xman-level1.md", true);
  request.send(null);
}
function App (){
    const [content, setContent] = React.useState("null");

    UpdateContainer(setContent);
    return (
      <Container fixed className="App" dangerouslySetInnerHTML={{__html: content}} style={{minWidth: "800px", height: "auto", marginLeft: "auto", marginRight: "auto", background: "#f5f5f5"}}>
          
      </Container>
    )
}
export default App;
