import BlogBriefBox from "./BlogBriefBox"

var testBlogBriefData1 = '{"title": "测试标题", "article": "测试内容…", "tags": ["programer", "华为", "996"], "interInfo": {"vote": "100", "comments": "999"}, "userInfo":{"name":"LegendLXH", "avatar":"lxhcat.jpg"}, "href": "/blogs/114514", "timestamp": "十分钟前"}';

function GetTimeLine()
{
    //由array.map函数来进行动态的批量JSX元素生成
    //数据由此处的AJAX请求来获得
    var bunchData = [testBlogBriefData1, testBlogBriefData1, testBlogBriefData1, testBlogBriefData1, testBlogBriefData1, testBlogBriefData1,testBlogBriefData1];
    return bunchData.map((blogText, index) =>{
        var blogData = JSON.parse(blogText);
        return BlogBriefBox(blogData["title"], blogData["article"], blogData["tags"], blogData["interInfo"], blogData["userInfo"], blogData["href"], blogData["timestamp"]);
    })
}

function TimeLine()
{
    return GetTimeLine();
}

export default TimeLine;