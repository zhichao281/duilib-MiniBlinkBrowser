#pragma once
#include <string>
#include <map>
using namespace std;
#include "../3rd/miniblink/wke.h"





#define EVENT_TICK_TIEMER_ID	42




typedef struct _rend_data
{
	RECT rt; //渲染的矩形区域
	void* pixels; //渲染的内存数据
	HDC hDC; //内存设备
	HBITMAP hBitmap; //位图

	_rend_data()
	{
		memset(&rt, 0, sizeof(RECT));
		pixels = NULL;
		hDC = ::CreateCompatibleDC(0);
		hBitmap = NULL;
	}

	~_rend_data()
	{
		if (hDC)
			DeleteDC(hDC);

		if (hBitmap)
			DeleteObject(hBitmap);
	}

}REND_DATA, *PRENDDATA;

class CWkeWebkitUI;

class IWkeCallback
{
public:
	virtual void OnWkeTitleChanged(CWkeWebkitUI* webView, LPCTSTR title) {}
	virtual void OnWkeURLChanged(CWkeWebkitUI* webView, LPCTSTR url) {}
	virtual void OnWkeAlertBox(CWkeWebkitUI* webView, LPCTSTR msg) {}
	virtual bool OnWkeNavigation(CWkeWebkitUI* webView, wkeNavigationType navigationType, LPCTSTR url) { return false; }


	virtual wkeWebView OnWkeCreateView(CWkeWebkitUI* webView, wkeNavigationType navigationType, const wkeString url, const wkeWindowFeatures* windowFeatures) { return NULL; }

	virtual void OnWkeDocumentReady(CWkeWebkitUI* webView) {}

	virtual bool  onLoadUrlBegin(CWkeWebkitUI* webView, void* param, const char* url, void *job) { return false; }
	
	virtual void OnWkeLoadingFinish(CWkeWebkitUI* webView, const LPCTSTR url, wkeLoadingResult result, LPCTSTR failedReason) {}
	   
	virtual LPCTSTR OnJS2Native(CWkeWebkitUI *pWeb, LPCTSTR lpMethod, LPCTSTR lpContent, void *pListenObj) { return NULL; }
};



class CWkeWebkitUI : public CControlUI
{
public:
	DECLARE_DUICONTROL(CWkeWebkitUI)

public:
	CWkeWebkitUI(void);
	~CWkeWebkitUI(void);

protected:
	virtual LPCTSTR	GetClass() const;

	virtual LPVOID GetInterface(LPCTSTR pstrName);

	//当控件被添加到容器后，由容器调用的函数。
	//在这里，整个Duilib程序框架已经完成，当需要做一些界面的初始操作时可以重写此函数，
	//常见的用法就是在此建立Win32子窗体并且封装它，
	virtual void DoInit();

	virtual void SetPos(RECT rc, bool bNeedUpdate = true);
	//控件的核心函数，他是控件的绘制处理函数，当Duilib底层要重新绘制这个控件，
	//或者控件自己调用Invalidata函数强制自己刷新时，这个函数就会被触发，
	//在这个函数里完成了各种状态下的背景前景绘制，背景色绘制，文本绘制，边框绘制。而这个函数会调用PaintBkColor、PaintBkImage、PaintStatusImage、PaintText、PaintBorder等函数来完成各个绘制步骤。所以你可以根据需求来决定重写DoPaint或者只重写某几个PaintXXX函数。DoPaint函数经常和DoEvent函数结合使用，DoEvent得到了某个消息后，改变控件的状态，然后调用Invalidate函数让控件重绘。
	virtual bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
	//控件的核心函数，他是消息处理函数，用来处理Duilib封装过的各个消息，
	//比如鼠标的移入移出、出现的悬停、单击双击、右击、滚轮滑动、获取焦点、设置光标等等。
	//所以如果你的控件需要修改这些行为，必须重写这个函数，具体的处理方法可以参考Duilib现有的控件。
	virtual void DoEvent(TEventUI& event);

	virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

public:

	//初始化webkit
	static void InitializeWebkit();

	//反初始化webkit
	static void UninitializeWebkit();

	//执行js代码
	void ExecuteJS(LPCTSTR lpJS);

public:


	wkeWebView GetWebView();

	//加载url
	void Navigate(LPCTSTR lpUrl);

	//加载html代码
	void LoadHtml(LPCTSTR lpHtml);

	//关闭webkit窗口
	void Close();

	//页面操作：后退、前进、停止、刷新
	void Back();

	void Forward();

	void Stop();

	void Reload();

	//设置主页
	void SetHomePage(LPCTSTR lpHomeUrl);

	//浏览主页
	void NavigateHomePage();

	//设置页面焦点
	void SetPageFocus();

public:
	//设置监听对象
	void SetListenObj(void *pListenObj);

	// 设置回调接口
	void SetWkeCallback(IWkeCallback* pWkeCallback);

	//设置因网络原因导致页面加载失败后的提示页面
	void SetErrorInfoPageUrl(LPCTSTR lpErrUrl);

private:
	// 回调事件
	static void WKE_CALL_TYPE OnWkeTitleChanged(wkeWebView webView, void* param, wkeString title);
	static void WKE_CALL_TYPE OnWkeURLChanged(wkeWebView webView, void* param, wkeString url);
	static void WKE_CALL_TYPE OnWkeAlertBox(wkeWebView webView, void* param, wkeString msg);
	static bool WKE_CALL_TYPE OnWkeNavigation(wkeWebView webView, void* param, wkeNavigationType navigationType, wkeString url);

	static wkeWebView(WKE_CALL_TYPE OnWkeCreateView)(wkeWebView webView, void* param, wkeNavigationType navigationType, const wkeString url, const wkeWindowFeatures* windowFeatures);

	static void WKE_CALL_TYPE OnWkeDocumentReady(wkeWebView webView, void* param);

	static bool WKE_CALL_TYPE onLoadUrlBegin(wkeWebView webView, void* param, const char* url, void *job);
	
	static void WKE_CALL_TYPE OnWkeLoadingFinish(wkeWebView webView, void* param, const wkeString url, wkeLoadingResult result, const wkeString failedReason);
	
	
	
	//内置一个js与本地的函数映射
	static jsValue JS_CALL JsToNative(jsExecState es);

	static jsValue WKE_CALL_TYPE onMsg(jsExecState es, void* param);

	static jsValue WKE_CALL_TYPE onShellExec(jsExecState es, void* param);

private:
	REND_DATA m_RendData;

	wkeWebView	 m_pWebView;

	TCHAR m_chHomeUrl[1024]; //主页的url

	TCHAR m_chCurPageUrl[1024]; //当前页面的url

	TCHAR m_chErrUrl[1024]; //错误提示页面的url

private:
	void *m_pListenObj; //监听对象

	IWkeCallback* m_pWkeCallback;	// 回调接口

	static map<wkeWebView, CWkeWebkitUI*> m_mapWke2UI; //建立Wke核心到WebkitUI的映射关系
};
