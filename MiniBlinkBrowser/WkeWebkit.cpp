#include "StdAfx.h"
#include "WkeWebkit.h"
#include <Windows.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "imm32.lib")
#include "../Common/String/NSString.h"

map<wkeWebView, CWkeWebkitUI*> CWkeWebkitUI::m_mapWke2UI;

LPCTSTR wkeGetStringT(wkeString str)
{
#ifdef _UNICODE
	return wkeGetStringW(str);
#else
	return wkeGetString(str);
#endif
}

IMPLEMENT_DUICONTROL(CWkeWebkitUI)
CWkeWebkitUI::CWkeWebkitUI(void)
{
	memset(m_chHomeUrl, 0, sizeof(m_chHomeUrl));
	memset(m_chCurPageUrl, 0, sizeof(m_chCurPageUrl));
	memset(m_chErrUrl, 0, sizeof(m_chErrUrl));

	m_pWebView = wkeCreateWebView();

	m_mapWke2UI[m_pWebView] = this;

	m_pListenObj = NULL;
	m_pWkeCallback = NULL;

	TCHAR modulePath[MAX_PATH] = { 0 };
	TCHAR curDir[MAX_PATH] = { 0 }, drive[_MAX_DRIVE] = { 0 }, dir[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, modulePath, MAX_PATH);
	_wsplitpath(modulePath, drive, dir, NULL, NULL);
	_tcscpy(curDir, drive), _tcscat(curDir, dir);
	_tcscpy(m_chErrUrl, L"file:///"), _tcscat(m_chErrUrl, curDir), _tcscat(m_chErrUrl, L"//resources//error//error.html");
	_tcscpy(m_chHomeUrl, L"file:///"), _tcscat(m_chHomeUrl, curDir), _tcscat(m_chHomeUrl, L"//resources//test.html");

	m_cursor = -1;
	

}

CWkeWebkitUI::~CWkeWebkitUI(void)
{
	map<wkeWebView, CWkeWebkitUI*>::const_iterator iter = m_mapWke2UI.find(m_pWebView);
	if (iter != m_mapWke2UI.end())
	{
		m_mapWke2UI.erase(iter);
	}


	wkeDestroyWebView(m_pWebView);
}

LPCTSTR CWkeWebkitUI::GetClass() const
{
	return _T("CWkeWebkitUI");
}


LPVOID CWkeWebkitUI::GetInterface(LPCTSTR pstrName)
{
	if (_tcscmp(pstrName, _T("WkeBrowser")) == 0)
		return static_cast<CWkeWebkitUI*>(this);
	else
		return CControlUI::GetInterface(pstrName);
}


void CWkeWebkitUI::DoInit()
{

	CControlUI::DoInit();
	HWND hWnd =m_pManager->GetPaintWindow();

	wkeSetHandle(m_pWebView, hWnd);

	// 设置名称
	wkeSetName(m_pWebView, NStr::T2ANSI(GetName()).c_str());


	// 初始化后回调接口
	wkeSetTransparent(m_pWebView, false); //通知无窗口模式下，webview开启透明模式。

	wkeOnTitleChanged(m_pWebView, OnWkeTitleChanged, this);

	wkeOnURLChanged(m_pWebView, OnWkeURLChanged, this);

	wkeOnNavigation(m_pWebView, OnWkeNavigation, this);

	wkeOnCreateView(m_pWebView, OnWkeCreateView, this);

	wkeOnDocumentReady(m_pWebView, OnWkeDocumentReady, this);

	wkeOnLoadUrlBegin(m_pWebView, onLoadUrlBegin, this);

	wkeOnLoadingFinish(m_pWebView, OnWkeLoadingFinish, this);

	wkeOnDownload(m_pWebView, OnWkeDownload, this);

	wkeOnPaintUpdated(m_pWebView, OnWkePaintUpdate, this);

	wkeOnAlertBox(m_pWebView, OnWkeAlertBox, this);


	// 设置UA
	wkeSetUserAgent(m_pWebView, "Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/69.0.2228.0 Safari/537.36");
	GetManager()->AddMessageFilter(this);
}

void CWkeWebkitUI::SetPos(RECT rc, bool bNeedUpdate/* = true*/)
{
	m_RendData.rt = rc;
	m_RendData.pixels = NULL;
	// 调整位置和尺寸
	CControlUI::SetPos(rc, bNeedUpdate);
	wkeResize(m_pWebView, rc.right - rc.left, rc.bottom - rc.top);
}

void CWkeWebkitUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	int* pInt = new int[10];
	CControlUI::SetAttribute(pstrName, pstrValue);
	if (_tcscmp(pstrName, _T("homepage")) == 0)
	{
		SetHomePage(pstrValue);
	}
}

LRESULT CWkeWebkitUI::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool & bHandled)
{
	CControlUI *current = GetManager()->FindControl(GetManager()->GetMousePos());
	if ( current != this)
	{
		return S_OK;
	}
	if (uMsg == WM_SETCURSOR)
	{
		bHandled = true;
		return S_OK;
	}
	//修改输入法的位置
	else if (uMsg == WM_IME_STARTCOMPOSITION)
	{
		const RECT controlPos = this->GetPos();
		wkeRect rect = wkeGetCaretRect(m_pWebView);
		HIMC hImc = ::ImmGetContext(GetManager()->GetPaintWindow());
		COMPOSITIONFORM Composition = { 0 };
		Composition.dwStyle = CFS_POINT | CFS_FORCE_POSITION;
		Composition.ptCurrentPos.x = rect.x + controlPos.left;
		Composition.ptCurrentPos.y = rect.y + controlPos.top;
		ImmSetCompositionWindow(hImc, &Composition);
		ImmReleaseContext(GetManager()->GetPaintWindow(), hImc);
		bHandled = TRUE;
		return S_OK;
	}
	return S_OK;
}

bool CWkeWebkitUI::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl)
{
	CControlUI::DoPaint(hDC, rcPaint, pStopControl);

	//if (m_RendData.pixels == NULL) {
	//	BITMAPINFO bi;
	//	memset(&bi, 0, sizeof(bi));
	//	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	//	bi.bmiHeader.biWidth = int(m_RendData.rt.right - m_RendData.rt.left);
	//	bi.bmiHeader.biHeight = -int(m_RendData.rt.bottom - m_RendData.rt.top);
	//	bi.bmiHeader.biPlanes = 1;
	//	bi.bmiHeader.biBitCount = 32;
	//	bi.bmiHeader.biCompression = BI_RGB;
	//	HBITMAP hbmp = ::CreateDIBSection(0, &bi, DIB_RGB_COLORS, &m_RendData.pixels, NULL, 0);
	//	SelectObject(m_RendData.hDC, hbmp);
	//	if (m_RendData.hBitmap) {
	//		DeleteObject(m_RendData.hBitmap);
	//	}
	//	m_RendData.hBitmap = hbmp;
	//}
	//wkePaint(m_pWebView, m_RendData.pixels, 0);
	//::BitBlt(hDC, m_RendData.rt.left, m_RendData.rt.top, m_RendData.rt.right - m_RendData.rt.left, m_RendData.rt.bottom - m_RendData.rt.top, m_RendData.hDC, 0, 0, SRCCOPY);



	HDC mb_hdc = wkeGetViewDC(m_pWebView);
	::BitBlt(hDC, rcPaint.left, rcPaint.top, rcPaint.right - rcPaint.left, rcPaint.bottom - rcPaint.top, mb_hdc, 0, 0, SRCCOPY);



	return true;



}

void CWkeWebkitUI::InitializeWebkit()
{
	// 加载mb的资源
	CDuiString strResourcePath = CPaintManagerUI::GetInstancePath();
	//CDuiString mbPath = strResourcePath + L"node.dll";
	CDuiString mbPath = strResourcePath + L"node_v8_4_8.dll";
	if (!::PathFileExists(mbPath))
	{
		::MessageBoxW(NULL, L"请把node.dll放exe目录下", L"错误", MB_OK);
		return ;
	}
	wkeSetWkeDllPath(mbPath);

	wkeInitialize();

	//绑定全局函数
	jsBindFunction("jsToNative", JsToNative, 2);

	wkeJsBindFunction("eMsg", &onMsg, nullptr, 5);
	wkeJsBindFunction("eShellExec", &onShellExec, nullptr, 3);
}

void CWkeWebkitUI::UninitializeWebkit()
{
	// 清理
	wkeFinalize();
}

void CWkeWebkitUI::ExecuteJS(LPCTSTR lpJS)
{
	wkeRunJS(m_pWebView, NStr::T2UTF8(lpJS).c_str());
}

void CWkeWebkitUI::updateCursor()
{

	int cursorInfo = wkeGetCursorInfoType(m_pWebView);
	if (m_cursor != cursorInfo) 
	{
		m_cursor = cursorInfo;
		HCURSOR curosr = ::LoadCursor(NULL, IDC_ARROW);
		switch (cursorInfo)
		{
		case WkeCursorInfoPointer:
			curosr = ::LoadCursor(NULL, IDC_ARROW);
			break;
		case WkeCursorInfoCross:
			curosr = ::LoadCursor(NULL, IDC_CROSS);
			break;
		case WkeCursorInfoHand:
			curosr = ::LoadCursor(NULL, IDC_HAND);
			break;
		case WkeCursorInfoIBeam:
			curosr = ::LoadCursor(NULL, IDC_IBEAM);
			break;
		case WkeCursorInfoWait:
			curosr = ::LoadCursor(NULL, IDC_WAIT);
			break;
		case WkeCursorInfoHelp:
			curosr = ::LoadCursor(NULL, IDC_HELP);
			break;
		case WkeCursorInfoEastResize:
			curosr = ::LoadCursor(NULL, IDC_SIZEWE);
			break;
		case WkeCursorInfoNorthResize:
			curosr = ::LoadCursor(NULL, IDC_SIZENS);
			break;
		case WkeCursorInfoNorthEastResize:
			curosr = ::LoadCursor(NULL, IDC_SIZENESW);
			break;
		case WkeCursorInfoNorthWestResize:
			curosr = ::LoadCursor(NULL, IDC_SIZENWSE);
			break;
		case WkeCursorInfoSouthResize:
			curosr = ::LoadCursor(NULL, IDC_SIZENS);
			break;
		case WkeCursorInfoSouthEastResize:
			curosr = ::LoadCursor(NULL, IDC_SIZENWSE);
			break;
		case WkeCursorInfoSouthWestResize:
			curosr = ::LoadCursor(NULL, IDC_SIZENESW);
			break;
		case WkeCursorInfoWestResize:
			curosr = ::LoadCursor(NULL, IDC_SIZEWE);
			break;
		case WkeCursorInfoNorthSouthResize:
			curosr = ::LoadCursor(NULL, IDC_SIZENS);
			break;
		case WkeCursorInfoEastWestResize:
			curosr = ::LoadCursor(NULL, IDC_SIZEWE);
			break;
		case WkeCursorInfoNorthEastSouthWestResize:
			curosr = ::LoadCursor(NULL, IDC_SIZEALL);
			break;
		case WkeCursorInfoNorthWestSouthEastResize:
			curosr = ::LoadCursor(NULL, IDC_SIZEALL);
			break;
		case WkeCursorInfoColumnResize:
		case WkeCursorInfoRowResize:
			curosr = ::LoadCursor(NULL, IDC_ARROW);
			break;
		default:
			break;
		}
		::SetCursor(curosr);
	}

}

void CWkeWebkitUI::DoEvent(TEventUI& event)
{
	RECT rc = GetPos();
	POINT pt = { event.ptMouse.x - rc.left, event.ptMouse.y - rc.top };
	static WkeCursorInfoType cursorInfo = WkeCursorInfoType::WkeCursorInfoPointer;
	switch (event.Type)
	{
	case UIEVENT_MOUSEENTER:
	case UIEVENT_MOUSELEAVE:
	case UIEVENT_MOUSEMOVE:
	{
		unsigned int flags = 0;
		if (event.wParam & MK_CONTROL)
			flags |= WKE_CONTROL;
		if (event.wParam & MK_SHIFT)
			flags |= WKE_SHIFT;
		if (event.wParam & MK_LBUTTON)
			flags |= WKE_LBUTTON;
		if (event.wParam & MK_MBUTTON)
			flags |= WKE_MBUTTON;
		if (event.wParam & MK_RBUTTON)
			flags |= WKE_RBUTTON;
		wkeFireMouseEvent(m_pWebView, WKE_MSG_MOUSEMOVE, pt.x, pt.y, flags);
		updateCursor();
	}
	break;
	case UIEVENT_BUTTONDOWN:
		wkeFireMouseEvent(m_pWebView, WKE_MSG_LBUTTONDOWN, pt.x, pt.y, event.wKeyState);
		break;
	case UIEVENT_BUTTONUP:
		wkeFireMouseEvent(m_pWebView, WKE_MSG_LBUTTONUP, pt.x, pt.y, event.wKeyState);
		break;
	case UIEVENT_RBUTTONDOWN:
		wkeFireMouseEvent(m_pWebView, WKE_MSG_RBUTTONDOWN, pt.x, pt.y, event.wKeyState);
		break;
	case UIEVENT_DBLCLICK:
		wkeFireMouseEvent(m_pWebView, WKE_MSG_LBUTTONDBLCLK, pt.x, pt.y, event.wKeyState);
		break;
	case UIEVENT_SCROLLWHEEL:
		wkeFireMouseWheelEvent(m_pWebView, pt.x, pt.y, event.wParam == SB_LINEUP ? 120 : -120, event.wKeyState);
		break;
	case UIEVENT_KEYDOWN:
		wkeFireKeyDownEvent(m_pWebView, event.chKey, event.lParam, false);
		break;
	case UIEVENT_KEYUP:
		wkeFireKeyUpEvent(m_pWebView, event.chKey, event.lParam, false);
		break;
	case UIEVENT_CHAR:
	{
		wkeFireKeyPressEvent(m_pWebView, event.chKey, event.lParam, false);
		break;
	}
	case UIEVENT_SETFOCUS:
		wkeSetFocus(m_pWebView);
		break;
	case UIEVENT_KILLFOCUS:
		wkeKillFocus(m_pWebView);
		break;
	case UIEVENT_SETCURSOR:
		return;


	case UIEVENT_CONTEXTMENU:
	{
		unsigned int flags = 0;
		if (event.wParam & MK_CONTROL)
			flags |= WKE_CONTROL;
		if (event.wParam & MK_SHIFT)
			flags |= WKE_SHIFT;
		wkeFireContextMenuEvent(m_pWebView, pt.x, pt.y, flags);
		break;
	}
	case UIEVENT_TIMER:
	/*	if (event.wParam == EVENT_TICK_TIEMER_ID) 
		{
			Invalidate();
		}*/
		break;
	default: break;
	}
	CControlUI::DoEvent(event);
}

wkeWebView CWkeWebkitUI::GetWebView()
{
	return m_pWebView;
}

void CWkeWebkitUI::Navigate(LPCTSTR lpUrl)
{
#ifdef _UNICODE
	wkeLoadW(m_pWebView, lpUrl);
#else
	wkeLoad(m_pWebView, lpUrl);
#endif	
}

void CWkeWebkitUI::LoadHtml(LPCTSTR lpHtml)
{
#ifdef _UNICODE
	wkeLoadHTMLW(m_pWebView, lpHtml);
#else
	wkeLoadHTML(m_pWebView, lpHtml);
#endif	
}

void CWkeWebkitUI::Close()
{
	wkeDestroyWebView(m_pWebView);
}

void CWkeWebkitUI::Back()
{
	if (wkeCanGoBack(m_pWebView))
		wkeGoBack(m_pWebView);
}

void CWkeWebkitUI::Forward()
{
	if (wkeCanGoForward(m_pWebView))
		wkeGoForward(m_pWebView);
}

void CWkeWebkitUI::Stop()
{
	wkeStopLoading(m_pWebView);
}

void CWkeWebkitUI::Reload()
{
	wkeReload(m_pWebView);
}

void CWkeWebkitUI::SetHomePage(LPCTSTR lpHomeUrl)
{
	_tcscpy(m_chHomeUrl, lpHomeUrl);
	Navigate(m_chHomeUrl);
}

void CWkeWebkitUI::SetErrorInfoPageUrl(LPCTSTR lpErrUrl)
{
	_tcscpy(m_chErrUrl, lpErrUrl);
}

void CWkeWebkitUI::NavigateHomePage()
{
	Navigate(m_chHomeUrl);
}

void CWkeWebkitUI::SetPageFocus()
{
	wkeSetFocus(m_pWebView);
}

void CWkeWebkitUI::SetListenObj(void *pListenObj)
{
	m_pListenObj = pListenObj;
}

void CWkeWebkitUI::SetWkeCallback(IWkeCallback* pWkeCallback)
{
	m_pWkeCallback = pWkeCallback;
}

void WKE_CALL_TYPE CWkeWebkitUI::OnWkePaintUpdate(wkeWebView webView, void* param, const HDC hdc, int x, int y, int cx, int cy)
{
	CWkeWebkitUI *pWkeUI = (CWkeWebkitUI*)param;
	if (!pWkeUI)	return;
	pWkeUI->Invalidate();

}


void WKE_CALL_TYPE CWkeWebkitUI::OnWkeTitleChanged(wkeWebView webView, void* param, wkeString title)
{
	CWkeWebkitUI *pWkeUI = (CWkeWebkitUI*)param;
	if (!pWkeUI)	return;
	if (pWkeUI->m_pWkeCallback) {
		return pWkeUI->m_pWkeCallback->OnWkeTitleChanged(pWkeUI, wkeGetStringT(title));
	}
}

void WKE_CALL_TYPE CWkeWebkitUI::OnWkeURLChanged(wkeWebView webView, void* param, wkeString url)
{
	wkeTempCallbackInfo* temInfo = wkeGetTempCallbackInfo(webView);
	if (::wkeIsMainFrame(webView, temInfo->frame))
	{
		CWkeWebkitUI *pWkeUI = (CWkeWebkitUI*)param;
		if (!pWkeUI)	return;
		if (pWkeUI->m_pWkeCallback)
		{
			return pWkeUI->m_pWkeCallback->OnWkeURLChanged(pWkeUI, wkeGetStringT(url));
		}
	}


}

void WKE_CALL_TYPE CWkeWebkitUI::OnWkeAlertBox(wkeWebView webView, void* param, wkeString msg)
{
	CWkeWebkitUI *pWkeUI = (CWkeWebkitUI*)param;
	if (!pWkeUI)	return;

	if (pWkeUI->m_pWkeCallback) {
		return pWkeUI->m_pWkeCallback->OnWkeAlertBox(pWkeUI, wkeGetStringT(msg));
	}
}

bool WKE_CALL_TYPE CWkeWebkitUI::OnWkeNavigation(wkeWebView webView, void* param, wkeNavigationType navigationType, wkeString url)
{
	CWkeWebkitUI *pWkeUI = (CWkeWebkitUI*)param;
	if (!pWkeUI)	return true;

	LPCTSTR pStrUrl = wkeGetStringT(url);
	if (!_tcsstr(pStrUrl, _T("error.html"))) {
		_tcscpy(pWkeUI->m_chCurPageUrl, pStrUrl);
	}

	if (pWkeUI->m_pWkeCallback) {
		return pWkeUI->m_pWkeCallback->OnWkeNavigation(pWkeUI, navigationType, pStrUrl);
	}

	return true;
}

wkeWebView WKE_CALL_TYPE CWkeWebkitUI::OnWkeCreateView(wkeWebView webView, void* param, wkeNavigationType navigationType, const wkeString url, const wkeWindowFeatures* windowFeatures)
{
	CWkeWebkitUI *pWkeUI = (CWkeWebkitUI*)param;
	if (!pWkeUI)	return NULL;

	if (pWkeUI->m_pWkeCallback)
	{
		return pWkeUI->m_pWkeCallback->OnWkeCreateView(pWkeUI, navigationType, url, windowFeatures);
	}

	return NULL;
}

void WKE_CALL_TYPE CWkeWebkitUI::OnWkeDocumentReady(wkeWebView webView, void* param)
{
	CWkeWebkitUI *pWkeUI = (CWkeWebkitUI*)param;
	if (!pWkeUI)	return;

	if (pWkeUI->m_pWkeCallback) {
		return pWkeUI->m_pWkeCallback->OnWkeDocumentReady(pWkeUI);
	}
}

void readJsFile(const wchar_t* path, std::vector<char>* buffer)
{
	HANDLE hFile = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile) {
		DebugBreak();
		return;
	}

	DWORD fileSizeHigh;
	const DWORD bufferSize = ::GetFileSize(hFile, &fileSizeHigh);

	DWORD numberOfBytesRead = 0;
	buffer->resize(bufferSize);
	BOOL b = ::ReadFile(hFile, &buffer->at(0), bufferSize, &numberOfBytesRead, nullptr);
	::CloseHandle(hFile);
	b = b;
}

//任何网络请求发起前会触发此回调
//注意：
//1，此回调功能强大，在回调里，如果对job设置了wkeNetHookRequest，则表示mb会缓存获取到的网络数据，并在这次网络请求 结束后调用wkeOnLoadUrlEnd设置的回调，同时传递缓存的数据。在此期间，mb不会处理网络数据。
//2，如果在wkeLoadUrlBeginCallback里没设置wkeNetHookRequest，则不会触发wkeOnLoadUrlEnd回调。
//3，如果wkeLoadUrlBeginCallback回调里返回true，表示mb不处理此网络请求（既不会发送网络请求）。返回false，表示mb依然会发送网络请求。
bool  WKE_CALL_TYPE CWkeWebkitUI::onLoadUrlBegin(wkeWebView webView, void* param, const char* url, void *job)
{
	const char kPreHead[] = "http://hook.test/";
	const char* pos = strstr(url, kPreHead);
	if (pos)
	{
		const utf8* decodeURL = wkeUtilDecodeURLEscape(url);
		if (!decodeURL)
			return false;
		std::string urlString(decodeURL);
		std::string localPath = urlString.substr(sizeof(kPreHead) - 1);

		std::wstring path = CPaintManagerUI::GetInstancePath().GetData()+ NStr::utf8ToUtf16(localPath);
		std::vector<char> buffer;

		readJsFile(path.c_str(), &buffer);

		wkeNetSetData(job, buffer.data(), buffer.size());

		return true;
	}
	else if (strncmp(url, "http://localhost:12222", 22) == 0)
	{
		wkeNetSetMIMEType(job, (char*)"text/html");
		wkeNetSetData(job, (char*)"\"test1111\"", 10);
		return true;
	}
	else if (strcmp(url, "http://www.baidu.com/") == 0) {
		wkeNetHookRequest(job);
	}

	return false;
}



void WKE_CALL_TYPE CWkeWebkitUI::OnWkeLoadingFinish(wkeWebView webView, void* param, const wkeString url, wkeLoadingResult result, const wkeString failedReason)
{

	CWkeWebkitUI *pWkeUI = (CWkeWebkitUI*)param;
	if (!pWkeUI)	return;

	//页面加载失败
	if (result == WKE_LOADING_FAILED)
	{
		pWkeUI->Navigate(pWkeUI->m_chErrUrl);
	}


	wkeTempCallbackInfo* temInfo = wkeGetTempCallbackInfo(webView);
	if (::wkeIsMainFrame(webView, temInfo->frame)) 
	{
		::wkeNetGetFavicon(webView, OnWkeNetGetFavicon, param);
	}


	if (pWkeUI->m_pWkeCallback) 
	{
		pWkeUI->m_pWkeCallback->OnWkeLoadingFinish(pWkeUI, wkeGetStringT(url), result, wkeGetStringT(failedReason));
	}
}
void WKE_CALL_TYPE CWkeWebkitUI::OnWkeNetGetFavicon(wkeWebView webView, void * param, const utf8 * url, wkeMemBuf * buf)
{
	CWkeWebkitUI *pWkeUI = (CWkeWebkitUI*)param;
	if (!pWkeUI)	return;

	if (pWkeUI->m_pWkeCallback && buf!=nullptr &&url!=nullptr)
	{
		pWkeUI->m_pWkeCallback->OnWkeNetGetFavicon(pWkeUI, url, buf);
	}

	return ;
}



bool WKE_CALL_TYPE CWkeWebkitUI::OnWkeDownload(wkeWebView webView, void * param, const char * url)
{
	
	CWkeWebkitUI *pWkeUI = (CWkeWebkitUI*)param;
	if (!pWkeUI)	return false;

	if (pWkeUI->m_pWkeCallback) {
		return pWkeUI->m_pWkeCallback->OnWkeDownload(pWkeUI, url);
	}
	return true;
}

jsValue WKE_CALL_TYPE  CWkeWebkitUI::onMsg(jsExecState es, void* param)
{
	int argCount = jsArgCount(es);
	if (argCount < 1)
		return jsUndefined();

	jsType type = jsArgType(es, 0);
	if (JSTYPE_STRING != type)
		return jsUndefined();

	jsValue arg0 = jsArg(es, 0);
	std::string msgOutput = "eMsg:";
	std::string msg = jsToTempString(es, arg0);
	msgOutput = msgOutput + msg;
	msgOutput += "\n";
	OutputDebugStringA(msgOutput.c_str());
	   	 
	//查找UI对象
	CWkeWebkitUI *pWkeUI = NULL;
	wkeWebView pWke = jsGetWebView(es);
	if (pWke) 
	{
		map<wkeWebView, CWkeWebkitUI*>::const_iterator iter = m_mapWke2UI.find(pWke);
		if (iter != m_mapWke2UI.end()) 
		{
			pWkeUI = iter->second;
		}
	}
	if (pWkeUI) 
	{
		int nArg = jsArgCount(es);
		if (nArg >0) {
			jsValue arg1 = jsArg(es, 0);

			if (jsIsString(arg1)) 
			{
			
#ifdef _UNICODE 
				wchar_t buf1[16 * 1024] = { 0 };
				wcsncpy(buf1, jsToTempStringW(es, arg1), 16 * 1024 - 1);

#else
				char buf1[16 * 1024] = { 0 };
				strncpy(buf1, jsToTempString(es, arg1), 16 * 1024 - 1);
			
#endif

				LPCTSTR lpArg1 = buf1;

				if (wcscmp(lpArg1, L"refresh") == 0) {
					//本地刷新
					pWkeUI->Navigate(pWkeUI->m_chCurPageUrl);
					return jsUndefined();
				}

				if (pWkeUI->m_pWkeCallback) {
					LPCTSTR lpRet = pWkeUI->m_pWkeCallback->OnJS2Native(pWkeUI, lpArg1, L"", pWkeUI->m_pListenObj);
					if (lpRet) {
#ifdef _UNICODE
						return jsStringW(es, lpRet);
#else
						return jsString(es, lpRet);
#endif
					}
				}

			}
		}
	}

	return jsUndefined();



}

jsValue WKE_CALL_TYPE CWkeWebkitUI::onShellExec(jsExecState es, void* param)
{
	if (0 == jsArgCount(es))
		return jsUndefined();
	jsValue arg0 = jsArg(es, 0);
	if (!jsIsString(arg0))
		return jsUndefined();

	jsValue arg1 = jsArg(es, 1);
	if (!jsIsString(arg1))
		return jsUndefined();

	std::string strName;
	strName = jsToTempString(es, arg0);

	std::string path;
	path = jsToTempString(es, arg1);

	if ("runEchars" == path) {
		//createECharsTest();
	}
	else if ("wkeBrowser" == path) {
		//wkeBrowserMain(nullptr, nullptr, nullptr, TRUE);
	}
	else{
		//system(path.c_str());
		WinExec(strName.c_str(), SW_SHOW);
	}
	
	path += "\n";
	OutputDebugStringA(path.c_str());

	return jsUndefined();
}

jsValue JS_CALL CWkeWebkitUI::JsToNative(jsExecState es)
{
	//查找UI对象
	CWkeWebkitUI *pWkeUI = NULL;
	wkeWebView pWke = jsGetWebView(es);
	if (pWke) {
		map<wkeWebView, CWkeWebkitUI*>::const_iterator iter = m_mapWke2UI.find(pWke);
		if (iter != m_mapWke2UI.end()) {
			pWkeUI = iter->second;
		}
	}
	if (pWkeUI) {
		int nArg = jsArgCount(es);
		if (nArg == 2)
		{
			jsValue arg1 = jsArg(es, 0);
			jsValue arg2 = jsArg(es, 1);
			int int0 = jsToInt(es, arg1);
			int int1 = jsToInt(es, arg2);
	

			return jsInt(int0+int1);
			if (jsIsString(arg1) && jsIsString(arg2)) {
				//需要保证两个参数都为字符串
#ifdef _UNICODE 
				wchar_t buf1[16 * 1024] = { 0 }, buf2[16 * 1024] = { 0 };
				wcsncpy(buf1, jsToTempStringW(es, arg1), 16 * 1024 - 1);
				wcsncpy(buf2, jsToTempStringW(es, arg2), 16 * 1024 - 1);
#else
				char buf1[16 * 1024] = { 0 }, buf2[16 * 1024] = { 0 };
				strncpy(buf1, jsToTempString(es, arg1), 16 * 1024 - 1);
				strncpy(buf2, jsToTempString(es, arg2), 16 * 1024 - 1);
#endif

				LPCTSTR lpArg1 = buf1;
				LPCTSTR lpArg2 = buf2;

				if (wcscmp(lpArg1, L"refresh") == 0) {
					//本地刷新
					pWkeUI->Navigate(pWkeUI->m_chCurPageUrl);
					return jsUndefined();
				}

				if (pWkeUI->m_pWkeCallback) {
					LPCTSTR lpRet = pWkeUI->m_pWkeCallback->OnJS2Native(pWkeUI, lpArg1, lpArg2, pWkeUI->m_pListenObj);
					if (lpRet) {
#ifdef _UNICODE
						return jsStringW(es, lpRet);
#else
						return jsString(es, lpRet);
#endif
					}
				}

			}
		}
	}

	return jsUndefined();
}
