控件仅封装了```wke.h```中的部分功能。

# 属性

你可以使用所有```CControl```的所有属性，同时，添加了```url```这个属性，你可以通过这个属性设置```miniblink```所显示的网页地址。

# API

> 提示：除```RunJs```和```SimulationAction```外的所有接口和miniblink一样，均不能跨线程调用。

仅对原接口进行封装，使其符合面向对象。

所有接口的用途可以参考[原文档](http://miniblink.net/doc-main.html)。

|名称    |wke对应接口    |    备注|
|---|---|------|
|static void SetWkeDllPath(LPCTSTR dllPath)|void wkeSetWkeDllPath(const wchar_t* dllPath)| |
|static void SetGlobalProxy(const wkeProxy* proxy) | void wkeSetProxy(const wkeProxy* proxy)| |
|static void Initialize()| void wkeInitialize() | |
|unsigned GetVersion() | unsigned int wkeVersion()| |
|wkeWebView GetWebView() | | 获取当前控件所使用的```wkeWebView```。获取到```wkeWebView```后，你可以调用```mb```的原生接口。|
|bool IsDocumentReady() | bool wkeIsDocumentReady(wkeWebView webView) | |
|void Reload() | bool wkeReload(wkeWebView webView) | |
|LPCWSTR GetTitle()|const utf8* wkeGetTitle(wkeWebView webView)/const wchar_t* wkeGetTitleW(wkeWebView webView)| |
|int GetWidth()|int wkeGetWidth(wkeWebView webView)| |
|int GetHeight()|int wkeGetHeight(wkeWebView webView)| |
|int GetContentWidth()| int wkeGetContentWidth(wkeWebView webView)| |
|int GetContentHeight()| int wkeGetContentHeight(wkeWebView webView) | |
|bool CanGoBack()| bool wkeCanGoBack(wkeWebView webView)| |
|bool GoBack()|bool wkeGoBack(wkeWebView webView) | |
|bool CanGoForward()| bool wkeCanGoForward(wkeWebView webView)| |
|bool GoForward()|bool wkeGoForward(wkeWebView webView)| |
|void EditorSelectAll()|void wkeEditorSelectAll(wkeWebView webView)| |
|void EditorUnSelect()|void wkeEditorUnSelect(wkeWebView webView)| |
|void EditorCopy()|void wkeEditorCopy(wkeWebView webView)| |
|void EditorCut()|void wkeEditorCut(wkeWebView webView)| |
|void EditorDelete()|void wkeEditorDelete(wkeWebView webView)| |
|void EditorUndo()|void wkeEditorUndo(wkeWebView webView)| |
|void EditorRedo()|void wkeEditorRedo(wkeWebView webView)| |
|LPCWSTR GetCookie()|const wchar_t * wkeGetCookieW(wkeWebView webView)/const utf8* wkeGetCookie(wkeWebView webView)| |
|void SetCookie(const char *cookie, const char *url = NULL)|void wkeSetCookie(wkeWebView webView, const utf8* url, const utf8* cookie)| |
|const char *GetUrl() const|const utf8* wkeGetURL(wkeWebView webView)| |
|void SetCookieEnabled(bool)|void wkeSetCookieEnabled(wkeWebView webView, bool enable)| |
|bool IsCookieEnabled()|bool wkeIsCookieEnabled(wkeWebView webView)| |
|void SetCookieFile(LPCWSTR file)|void wkeSetCookieJarFullPath(wkeWebView webView, const WCHAR* path)| |
|void SetLocalStoragePath(LPCWSTR file)|void wkeSetLocalStorageFullPath(wkeWebView webView, const WCHAR* path)| |
|void SetZoom(float factor)|void wkeSetZoomFactor(wkeWebView webView, float factor)| |
|float GetZoom()|float wkeGetZoomFactor(wkeWebView webView)| |
|void SetProxy(const wkeProxy* proxy)|void wkeSetViewProxy(wkeWebView webView, wkeProxy* proxy)| |
|void SetMemoryCacheEnable(bool)|void wkeSetMemoryCacheEnable(wkeWebView webView, bool b)| |
|void SetTouchEnabled(bool)|void wkeSetTouchEnabled(wkeWebView webView, bool b)| |
|void SetMouseEnabled(bool)|void wkeSetMouseEnabled(wkeWebView webView, bool b)| |
|void SetNavigationToNewWindowEnable(bool)|void wkeSetNavigationToNewWindowEnable(wkeWebView webView, bool b)| |
|void SetCspCheckEnable(bool)| void wkeSetCspCheckEnable(wkeWebView webView, bool b)| |
|void SetNpapiPluginsEnabled(bool)|void wkeSetNpapiPluginsEnabled(wkeWebView webView, bool b)| |
|void SetHeadlessEnabled(bool)|void wkeSetHeadlessEnabled(wkeWebView webView, bool b)| |
|void SetDebugConfig(const char*, const char*)|void wkeSetDebugConfig(wkeWebView webView, const char* debugString, const char* param)| |
|void SetTransparent(bool)|void wkeSetTransparent(wkeWebView webView, bool transparent)| |
|void SetUserAgent(LPCWSTR userAgent)|void wkeSetUserAgent(wkeWebView webView, const utf8* userAgent) /void wkeSetUserAgentW(wkeWebView webView, const wchar_t* userAgent)| |
|const char *GetUserAgent() const|const utf8* wkeGetUserAgent(wkeWebView webView)| |
|void SetHtml(const char* html, const char* baseUrl = NULL)|void wkeLoadHTML(wkeWebView webView, const utf8* html)/void wkeLoadHtmlWithBaseUrl(wkeWebView webView, const utf8* html, const utf8* baseUrl)|若```baseUrl```不为```NULL```，则调用```wkeLoadHtmlWithBaseUrl```。|
|void RunJs(const char* js)| |运行一段JS，不需要返回结果。|
|void RunJs(const char* js, int* result)||运行一段JS，返回一个int的结果，参数```int* result```用于存储返回结果。|
|void RunJs(const char* js, double* result)| |运行一段JS，返回一个double的结果，参数```double* result```用于存储返回结果。|
|void RunJs(const char* js, bool* result)| |运行一段JS，返回一个bool的结果，参数```bool* result```用于存储返回结果。|
|void RunJs(const char* js, char* result, int len)| |运行一段JS，返回字符串，参数```char* result```用于存储返回结果。参数```len```描述result的长度，以免写入结果时越界。|
|void RunJs(const char* js, wchar_t* result, int len)| | 运行一段JS，返回宽字符串，参数```wchar_t* result```用于存储返回结果。参数```len```描述result的长度，以免写入结果时越界。|
|void SetSize(int width, int height)| |设置```wkeWebView```的宽和高。|
|void SetDeviceParameter(const char* device, const char* paramStr, int paramInt, float paramFloat)|void wkeSetDeviceParameter(wkeWebView webView, const char* device, const char* paramStr, int paramInt, float paramFloat)| |
|void SetSettings(const wkeViewSettings* settings)| void wkeConfigure(const wkeSettings* settings)| |
|void SimulationAction(MB_ACTION_ITEM* action)| | 模拟一个鼠标或键盘动作。|
|void FlushCookie()| |将内存的中cookie写入到磁盘中。|
|void Goto(LPCTSTR url)| |跳转到指定地址，若```url```不是一个```http```或```https```开头的网址，将调用```void wkeLoadFile(wkeWebView webView, const utf8* filename)```加载本地文件。|

