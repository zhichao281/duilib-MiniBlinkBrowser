# 环境配置

假设你已经配置好了```DuiLib```环境。

# 快速开始

## 编写```xml```文件

在项目目录添加文件```main.xml```，内容如下：

```xml
<?xml version="1.0" encoding="utf-8" standalone="yes" ?>
<Window size="800,600" caption="0,0,0,32" roundcorner="4,4">
   <WkeWebkit name="wkeTest" homepage="http://hook.test/resources/view/index.html"/>
</Window>
```

## 创建窗口类
首先在duilib的InitResource接口中
// 注册控件
	REGIST_DUICONTROL(CWkeWebkitUI);
	
同时
	CWkeWebkitUI::InitializeWebkit();

此时，你的窗口中已经能正常浏览网页了。
