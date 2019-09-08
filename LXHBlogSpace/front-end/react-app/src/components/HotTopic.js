import React from 'react';
import ReactDOM from 'react-dom';
import "./css/HotTopic.css"

import List from "@material-ui/core/List"
import ListSubheader from "@material-ui/core/ListSubheader"
import ListItem from "@material-ui/core/ListItem"
import Chip from "@material-ui/core/Chip"

// eslint-disable-next-line
import Icon from "@material-ui/core/Icon"

// eslint-disable-next-line
function RenderHotTopic(className){
    var request = new XMLHttpRequest();
    request.onreadystatechange = function(){
        if (request.readyState === 4 && request.status === 200)
        {
            var topic = JSON.parse(request.responseText);
            ReactDOM.render(
                HotTopic(topic),
                document.getElementsByClassName(className)[0]
            );
        }
    }

    request.open("GET", "/api/v1/GetHotTopic");
    request.send(null);
}


function HotTopic(param){
    if (param.ecode !== 0)
    {
        param = JSON.parse('{"ecode": -1, "topics": ["热点加载中","","","","","","","","",""]}');
        RenderHotTopic("recommand-panel");
    }

    console.log(param["topics"]);
    return (
        <>
            <List>
                <ListSubheader component="div" disableSticky="false">热点博文</ListSubheader>
                {
                    param["topics"].map((elements, index) =>{
                        return (
                            <ListItem button className="topic-list-elements">
                                <Chip style={{borderRadius: 0, width: 25, height: 25}} label={index + 1} />
                                <div className="topic-label">
                                    {elements}
                                </div>
                            </ListItem>
                        )
                    })
                }
            </List>
            
        </>
    );
}

export default HotTopic;