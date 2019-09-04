import React from 'react';
import "./css/HotTopic.css"

import List from "@material-ui/core/List"
import ListSubheader from "@material-ui/core/ListSubheader"
import ListItem from "@material-ui/core/ListItem"
import Chip from "@material-ui/core/Chip"
// eslint-disable-next-line
import Icon from "@material-ui/core/Icon"
function GetHotTopic(){
    var topic = ["测试热点-1","测试热点-1","测试热点-1","测试热点-1","测试热点-1","测试热点-1","测试热点-1","测试热点-1","测试热点-1","测试热点-1"];
    return topic;
}

function HotTopic(){
    return (
        <div className="recommand-panel">
            <List>
                <ListSubheader component="div" disableSticky="false">热点博文</ListSubheader>
                {
                    GetHotTopic().map((element, index) =>{
                        return (
                            <ListItem button className="topic-list-elements">
                                <Chip style={{borderRadius: 0, width: 25, height: 25}} label={index + 1} />
                                <div className="topic-label">
                                    {element}
                                </div>
                            </ListItem>
                        )
                    })
                }
            </List>
            
        </div>
    );
}

export default HotTopic;