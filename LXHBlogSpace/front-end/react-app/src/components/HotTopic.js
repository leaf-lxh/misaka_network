import React from 'react';
import "./css/HotTopic.css"

import List from "@material-ui/core/List"
import ListSubheader from "@material-ui/core/ListSubheader"
import ListItem from "@material-ui/core/ListItem"
import Chip from "@material-ui/core/Chip"

class HotTopic extends React.Component
{
    constructor(props)
    {
        super(props);
        var param = JSON.parse('{"list_len": 10, "list": [{"article_id": "/", "title": "加载中"}]}');
		this.state = {
            srcListPanel:
            param["list"].map((elements, index) =>{
                return (
                    <ListItem button className="topic-list-elements">
                        <Chip style={{borderRadius: 0, width: 25, height: 25}} label=" " />
                        <div className="topic-label">
                            {elements.title}
                        </div>
                    </ListItem>
                )
            })
        }
	}

	render()
	{
    	return (
        	<>
            	<List>
                	<ListSubheader component="div" disableSticky="false">友情链接</ListSubheader>
                	{
                    	this.state.srcListPanel
                	}
            	</List>
        	</>
    	)
    }
    
    componentDidMount()
    {
        fetch("/api/v1/content/GetFriendLink")
        .then(res=>{
            if (res.ok)
            {
                return res.json()
            }
        })
        .then(res=>{
            if (res !== undefined)
            {
                var articleList = res;
                if (articleList.length === 0)
                {
                    this.setState({
                        srcListPanel:
                                <ListItem button className="topic-list-elements">
                                    <Chip style={{borderRadius: 0, width: 25, height: 25}} label="-" />
                                    <div className="topic-label">
                                        暂无
                                    </div>
                                </ListItem>
                    })
                }
                else
                {
                    this.setState({srcListPanel:
                        articleList.map((elements, index) =>{
                            return (
                                <ListItem button className="topic-list-elements" onClick={()=>{window.open(elements.href)}}>
                                    <Chip style={{borderRadius: 0, width: 25, height: 25}} label={index + 1} />
                                    <div className="topic-label">
                                        {elements.title}
                                    </div>
                                </ListItem>
                            )
                        })
                    })
                }
                
            }
        })

    }
}

export default HotTopic;
