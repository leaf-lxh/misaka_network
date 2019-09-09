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
        console.log(str);
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

      hookFunc(md.render(article).replace(/<code(.*?)>/img, (str, g1)=>{
        var match = g1.match(/(.*?)class="(.*?)"(.*?)/);
        if (match)
        {

          return "<code" + match[1] + "class='hljs " + match[2] + "'" + match[3];
        }
        else
        {
          return str.replace(/<code(.*?)>/, (str, g1)=>{
            return "<code class='hljs unmarked-code'" + g1 + ">";
          })
        }
      }));
    }
  }

  request.open("GET", "/fuckshit.md", true);
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
