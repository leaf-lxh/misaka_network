import React from 'react';
// eslint-disable-next-line
import { makeStyles } from '@material-ui/core/styles'

import BlogBriefBox from './BlogBriefBox'

var testBlogBriefData1 = '{"title": "测试标题", "article": "测试内容…                                                                   ", "tags": ["programer", "华为", "996"], "interInfo": {"vote": "100", "comments": "999"}, "userInfo":{"name":"LegendLXH", "avatar":"lxhcat.jpg"}, "href": "/blogs/114514"}';

var blog = '{"title": "系统运维从修补到宕机", "article": "目录：Linux服务 openssh-server", "tags": ["系统运维", "Linux系统加固"], "interInfo": {"vote": "100", "comments": "999"}, "userInfo":{"name":"LegendLXH", "avatar":"lxhcat.jpg"}, "href": "/blogs"}';
var bunchData = [blog, testBlogBriefData1, testBlogBriefData1, testBlogBriefData1, testBlogBriefData1, testBlogBriefData1, testBlogBriefData1, testBlogBriefData1, testBlogBriefData1, testBlogBriefData1, testBlogBriefData1, testBlogBriefData1, testBlogBriefData1, testBlogBriefData1, testBlogBriefData1, testBlogBriefData1, testBlogBriefData1, testBlogBriefData1,testBlogBriefData1,testBlogBriefData1];
function GetBlogBrief(){
    //由array.map函数来进行动态的批量JSX元素生成
    //数据由此处的AJAX请求来获得
    //var request = new XMLHttpRequest();

    return bunchData.map((blogText, index) =>{
        var blogData = JSON.parse(blogText);
        return BlogBriefBox(blogData["title"], blogData["article"], blogData["tags"], blogData["interInfo"], blogData["userInfo"], blogData["href"]);
    })
}


function BlogBrief (){
    return(
        <>
            <GetBlogBrief />
        </>
    )
}

export default BlogBrief;