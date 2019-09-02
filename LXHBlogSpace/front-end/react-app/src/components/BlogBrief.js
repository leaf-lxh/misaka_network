import React from 'react';
// eslint-disable-next-line
import { makeStyles } from '@material-ui/core/styles'

import BlogBriefBox from './BlogBriefBox'

var testBlogBriefData1 = '{"title": "这里是测试标题，所以一定要长", "article": "以前我有一个做通信的朋友，说死也不要去华为。 我问他为什么。 他说：7*24半夜爬起来修bug。 说真的我是理解的，程序员嘛，哪能没有bug。 但是每天，半夜锟斤拷烫烫烫烫烫烫烫烫烫烫烫烫烫烫烫…", "tags": ["programer", "华为", "996"], "interInfo": {"vote": "100", "comments": "999"}, "userInfo":{"name":"LegendLXH", "avatar":"lxhcat.jpg"}}';
var testBlogBriefData2 = '{"title": "This is the test title, so the length of it...", "article": "以前我有一个做通信的朋友，说死也不要去华为。 我问他为什么。 他说：7*24半夜爬起来修bug。 说真的我是理解的，程序员嘛，哪能没有bug。 但是每天，半夜锟斤拷烫烫烫烫烫烫烫烫烫烫烫烫烫烫烫…", "tags": ["programer", "华为", "996"], "interInfo": {"vote": "5", "comments": "20"}, "userInfo":{"name":"LegendLXH", "avatar":"lxhcat.jpg"}}';
var testBlogBriefData3 = '{"title": "Test title这是测试标题", "article": "之前已经有一些答案提到，在美国建国时，欧洲的政治思想已经走出了家长制。当时的政治思想认为每一个个人不再受到家庭和宗族的束缚，而是以一个个独立的个人参…", "tags": ["programer", "华为", "996"], "interInfo": {"vote": "0", "comments": "83"}, "userInfo":{"name":"LegendLXH", "avatar":"lxhcat.jpg"}}';
var testBlogBriefData4 = '{"title": "又遇到考试般的面试题（golang）", "article": "不是有没有可能的问题。问题的前半句话基本是事实。后半句话是部分的事实，有多准确，受其他因素限制。 正常呼吸产生的氧自由基对有机体的杀伤效应是确凿无误的…", "tags": ["programer", "华为", "996"], "interInfo": {"vote": "10 K", "comments": "233"}, "userInfo":{"name":"LegendLXH", "avatar":"lxhcat.jpg"}}';
var testBlogBriefData5 = '{"title": "方舟编译器对开发者有什么好处？", "article": "以前我有一个做通信的朋友，说死也不要去华为。 我问他为什么。 他说：7*24半夜爬起来修bug。 说真的我是理解的，程序员嘛，哪能没有bug。 但是每天，半夜锟斤拷烫烫烫烫烫烫烫烫烫烫烫烫烫烫烫…", "tags": ["programer", "华为", "996"], "interInfo": {"vote": "5.3 K", "comments": "1,135"}, "userInfo":{"name":"LegendLXH", "avatar":"lxhcat.jpg"}}';
var testBlogBriefData6 = '{"title": "开机卡死第一屏(连 REC 都进不去)关机充电模式又正常", "article": "以前我有一个做通信的朋友，说死也不要去华为。 我问他为什么。 他说：7*24半夜爬起来修bug。 说真的我是理解的，程序员嘛，哪能没有bug。 但是每天，半夜锟斤拷烫烫烫烫烫烫烫烫烫烫烫烫烫烫烫…", "tags": ["programer", "华为", "996"], "interInfo": {"vote": "0", "comments": "999"}, "userInfo":{"name":"LegendLXH", "avatar":"lxhcat.jpg"}}';
var testBlogBriefData7 = '{"title": "有从重庆去 google 开发者大会的朋友么", "article": "以前我有一个做通信的朋友，说死也不要去华为。 我问他为什么。 他说：7*24半夜爬起来修bug。 说真的我是理解的，程序员嘛，哪能没有bug。 但是每天，半夜锟斤拷烫烫烫烫烫烫烫烫烫烫烫烫烫烫烫…", "tags": ["programer", "华为", "996"], "interInfo": {"vote": "100 K", "comments": "13,123"}, "userInfo":{"name":"LegendLXH", "avatar":"lxhcat.jpg"}}';
var testBlogBriefData8 = '{"title": "Test title这是测试标题", "article": "以前我有一个做通信的朋友，说死也不要去华为。 我问他为什么。 他说：7*24半夜爬起来修bug。 说真的我是理解的，程序员嘛，哪能没有bug。 但是每天，半夜锟斤拷烫烫烫烫烫烫烫烫烫烫烫烫烫烫烫…", "tags": ["programer", "华为", "996"], "interInfo": {"vote": "888", "comments": "999"}, "userInfo":{"name":"LegendLXH", "avatar":"lxhcat.jpg"}}';

var bunchData = [testBlogBriefData1, testBlogBriefData2, testBlogBriefData3, testBlogBriefData4, testBlogBriefData5, testBlogBriefData6, testBlogBriefData7, testBlogBriefData8];
function GetBlogBrief()
{
    //由array.map函数来进行动态的批量JSX元素生成
    return bunchData.map((blogText, index) =>{
        var blogData = JSON.parse(blogText);
        return BlogBriefBox(blogData["title"], blogData["article"], blogData["tags"], blogData["interInfo"], blogData["userInfo"]);
    })
}

function BlogBrief (){
    return(
        <>
            <GetBlogBrief>

            </GetBlogBrief>
        </>
    )
}

export default BlogBrief;