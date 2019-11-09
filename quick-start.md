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

在项目中创建窗口类```CDuiFrameWnd```，继承自```WindowImplBase```。

由于该控件属于自定义控件，需要添加自定义的```xml```标签解析，在```CDuiFrameWnd```类的```CreateControl```中添加如下代码：
```c++
CControlUI* CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	if (_tcscmp(pstrClass, _T("Miniblink")) == 0)
	{
		DuiLib::CMiniblink *web = new DuiLib::CMiniblink();
		return web;
	}
	return NULL;
}
```

此时，你的窗口中已经能正常浏览网页了。
