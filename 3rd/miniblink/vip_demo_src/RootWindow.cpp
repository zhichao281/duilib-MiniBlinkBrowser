#include <windows.h>
#include <Shlwapi.h>
#include <xstring>
#include <vector>
#include "RootWindow.h"
#include "Resource.h"
#include "HideWndHelp.h"

std::string utf16ToUtf8(LPCWSTR lpszSrc)
{
    std::string sResult;
    if (lpszSrc != NULL) {
        int  nUTF8Len = WideCharToMultiByte(CP_UTF8, 0, lpszSrc, -1, NULL, 0, NULL, NULL);
        char* pUTF8 = new char[nUTF8Len + 1];
        if (pUTF8 != NULL) {
            ZeroMemory(pUTF8, nUTF8Len + 1);
            WideCharToMultiByte(CP_UTF8, 0, lpszSrc, -1, pUTF8, nUTF8Len, NULL, NULL);
            sResult = pUTF8;
            delete[] pUTF8;
        }
    }
    return sResult;
}

std::wstring utf8ToUtf16(const std::string& utf8)
{
    std::wstring utf16;
    size_t n = ::MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), utf8.size(), nullptr, 0);
    if (0 == n)
        return L"";
    std::vector<wchar_t> wbuf(n);
    ::MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), utf8.size(), &wbuf[0], n);
    utf16.resize(n + 5);
    utf16.assign(&wbuf[0], n);
    return utf16;
}

bool saveDumpFile(const std::wstring& url, const char* buffer, unsigned int size)
{
    HANDLE hFile = NULL;
    bool   bRet = false;

    hFile = CreateFileW(url.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (!hFile || INVALID_HANDLE_VALUE == hFile)
        return false;

    DWORD numberOfBytesWritten = 0;
    ::WriteFile(hFile, buffer, size, &numberOfBytesWritten, NULL);
    ::CloseHandle(hFile);
    bRet = true;
    

    return bRet;
}

void setUserDataPtr(HWND hWnd, void* ptr)
{
    ::SetLastError(ERROR_SUCCESS);
    LONG_PTR result = ::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(ptr));
}

// Return the window's user data pointer.
template <typename T>
T getUserDataPtr(HWND hWnd)
{
    return reinterpret_cast<T>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
}

WNDPROC setWndProcPtr(HWND hWnd, WNDPROC wndProc)
{
    WNDPROC old = reinterpret_cast<WNDPROC>(::GetWindowLongPtr(hWnd, GWLP_WNDPROC));
    LONG_PTR result = ::SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(wndProc));
    return old;
}

int RootWindow::m_globalCount = 0;

RootWindow::RootWindow()
{
    m_hWnd = nullptr;
    m_mbView = nullptr;
    //m_initUrl = nullptr;
    m_font = nullptr;
    m_backHwnd = nullptr;
    m_forwardHwnd = nullptr;
    m_reloadHwnd = nullptr;
    m_stopHwnd = nullptr;
    m_editHwnd = nullptr;
    m_hbrush = nullptr;

    m_globalCount++;
}

RootWindow::~RootWindow()
{
    m_globalCount--;

    if (0 == m_globalCount)
        ::PostThreadMessage(::GetCurrentThreadId(), WM_QUIT, 0, 0);
}

int testIeMain(HINSTANCE hInstance);

void MB_CALL_TYPE onGetMHTMLCallback(mbWebView webView, void* param, const utf8* mhtml)
{
    OutputDebugStringA("onGetMHTMLCallback\n");
}

void MB_CALL_TYPE onRunJsCallback(mbWebView webView, void* param, mbJsExecState es, mbJsValue v)
{
    OutputDebugStringA("onRunJsCallback\n");
}

static void MB_CALL_TYPE onUrlRequestWillRedirectCallback(mbWebView webView, void* param, mbWebUrlRequestPtr oldRequest, mbWebUrlRequestPtr request, mbWebUrlResponsePtr redirectResponse)
{
}

static void MB_CALL_TYPE onUrlRequestDidReceiveResponseCallback(mbWebView webView, void* param, mbWebUrlRequestPtr request, mbWebUrlResponsePtr response)
{
}

static void MB_CALL_TYPE onUrlRequestDidReceiveDataCallback(mbWebView webView, void* param, mbWebUrlRequestPtr request, const char* data, int dataLength)
{
    OutputDebugStringA("onUrlRequestDidReceiveDataCallback\n");
}

static void MB_CALL_TYPE onUrlRequestDidFailCallback(mbWebView webView, void* param, mbWebUrlRequestPtr request, const utf8* error)
{
     OutputDebugStringA("onUrlRequestDidFailCallback\n");
}

static void MB_CALL_TYPE onUrlRequestDidFinishLoadingCallback(mbWebView webView, void* param, mbWebUrlRequestPtr request, double finishTime)
{
    OutputDebugStringA("onUrlRequestDidFinishLoadingCallback\n");
}

void RootWindow::onTestNodejs()
{
    std::vector<wchar_t> path;
    path.resize(MAX_PATH + 1);
    memset(&path.at(0), 0, sizeof(wchar_t) * (MAX_PATH + 1));

    ::GetModuleFileNameW(nullptr, &path.at(0), MAX_PATH);
    ::PathRemoveFileSpecW(&path.at(0));

    ::PathAppend(&path.at(0), L"..\\test_nodejs.html");

    std::string url = utf16ToUtf8(&path.at(0));

    for (size_t i = 0; i < url.size(); ++i) {
        if ('\\' == url[i])
            url[i] = '/';
    }
    url.insert(0, "file:///");

    ::mbLoadURL(m_mbView, url.c_str());
}

const UINT kBaseId = 100;
const UINT kTestNodejsId = 101;

LRESULT RootWindow::hideWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    UINT id = LOWORD(wParam);
    if (WM_COMMAND != uMsg)
        return ::DefWindowProc(hwnd, uMsg, wParam, lParam);
    
    if (kBaseId + 0 == id) {
        onShowDevtools();
    } else if (kBaseId + 1 == id) {
        onTestNodejs();
    }

    return ::DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void RootWindow::onMoreCommand()
{
    POINT pt = { 0 };
    ::GetCursorPos(&pt);
    if (m_hMenu) {
        ::SetForegroundWindow(m_hideWndHelp->getWnd());
        ::TrackPopupMenu(m_hMenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, m_hideWndHelp->getWnd(), NULL);
        return;
    }

    RootWindow* self = this;
    m_hideWndHelp = new HideWndHelp(L"HideParentWindowClass", [self](HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) -> LRESULT {
        return self->hideWndProc(hWnd, uMsg, wParam, lParam);
    });

    m_hMenu = ::CreatePopupMenu();

    MENUITEMINFO info = { 0 };
    info.cbSize = sizeof(MENUITEMINFO);

    UINT count = 0;

    std::wstring labelStr(L"´ò¿ªdevtools");
    info.fMask |= MIIM_STRING | MIIM_ID;
    info.cch = labelStr.size();
    info.dwTypeData = const_cast<LPWSTR>(labelStr.c_str());
    info.wID = kBaseId + count;
    ::InsertMenuItem(m_hMenu, count, TRUE, &info);
    count++;

    labelStr = L"²âÊÔnodejs";
    info.fMask |= MIIM_STRING | MIIM_ID;
    info.cch = labelStr.size();
    info.dwTypeData = const_cast<LPWSTR>(labelStr.c_str());
    info.wID = kBaseId + count;
    ::InsertMenuItem(m_hMenu, count, TRUE, &info);
    count++;

    ::SetForegroundWindow(m_hideWndHelp->getWnd());
    ::TrackPopupMenu(m_hMenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, m_hideWndHelp->getWnd(), NULL);

    return;
}

void RootWindow::onShowDevtools()
{
    //mbUtilSerializeToMHTML(m_mbView, onGetMHTMLCallback, nullptr);

    //mbRunJs(m_mbView, (mbWebFrameHandle)2, "document.documentElement.outerHTML;", true, onRunJsCallback, nullptr, nullptr);

//     mbWebUrlRequestPtr request = mbNetCreateWebUrlRequest("https://baidu.com", "GET", "");
// 
//     mbUrlRequestCallbacks callbacks = {
//         onUrlRequestWillRedirectCallback,
//         onUrlRequestDidReceiveResponseCallback,
//         onUrlRequestDidReceiveDataCallback,
//         onUrlRequestDidFailCallback,
//         onUrlRequestDidFinishLoadingCallback
//     };
//     int netRequestId = mbNetStartUrlRequest(m_mbView, request, this, &callbacks);

    std::vector<wchar_t> path;
    path.resize(MAX_PATH + 1);
    memset(&path[0], 0, sizeof(wchar_t) * (MAX_PATH + 1));
    ::GetModuleFileNameW(nullptr, &path[0], MAX_PATH);
    ::PathRemoveFileSpecW(&path[0]);

    ::PathAppendW(&path[0], L"front_end\\inspector.html");

    std::string pathA = "E:/mycode/miniblink49/trunk/third_party/WebKit/Source/devtools/front_end/inspector.html";
    if (::PathFileExistsA(pathA.c_str())) {
        mbSetDebugConfig(m_mbView, "showDevTools", pathA.c_str());
        return;
    }

    if (::PathFileExistsW(&path[0])) {
        pathA = utf16ToUtf8(&path[0]);
        mbSetDebugConfig(m_mbView, "showDevTools", pathA.c_str());
    }
}

//////////////////////////////////////////////////////////////////////////

bool RootWindow::onCommand(UINT id) {
    switch (id) {
    case IDC_NAV_BACK:   // Back button
        mbGoBack(m_mbView);
        return true;
    case IDC_NAV_FORWARD:  // Forward button
        mbGoForward(m_mbView);
        return true;
    case IDC_NAV_RELOAD:  // Reload button
        mbReload(m_mbView);
        return true;
    case IDC_NAV_STOP:  // Stop button
        mbStopLoading(m_mbView);
        return true;
    case IDC_NAV_TEST:
        onMoreCommand();

//////////////////////////////////////////////////////////////////////////
//         mbRunJsByFrame(m_mbView, mbWebFrameGetMainFrame(m_mbView),
//             "var htmlObj = document.getElementById(\"ne_wrap\");\n"
//             "var bodyObj =document.getElementsByTagName('body');\n"
//             "var computedStyle = document.defaultView.getComputedStyle(htmlObj, null);\n"
//             "console.log('htmlObj:' + computedStyle.overflowY);\n"
//             "computedStyle = document.defaultView.getComputedStyle(bodyObj[0], null);\n"
//             "console.log('bodyObj:' + computedStyle.overflowY);\n"
//             , true);
// 
//         static int count = 0;
//         if ((count % 2) == 0) {
//             mbInsertCSSByFrame(m_mbView, mbWebFrameGetMainFrame(m_mbView), "html { overflow:hidden; }");
//             OutputDebugStringA("hidden! \n");
//         } else {
//             mbInsertCSSByFrame(m_mbView, mbWebFrameGetMainFrame(m_mbView), "html { overflow:visible; }");
//             OutputDebugStringA("visible! \n");
//         }
//         count++;
//////////////////////////////////////////////////////////////////////////
        return true;
    }

    return false;
}

LRESULT CALLBACK rootWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

static void registerRootClass(HINSTANCE hInstance, const std::wstring& window_class, HBRUSH background_brush)
{
    // Only register the class one time.
    static bool classRegistered = false;
    if (classRegistered)
        return;
    classRegistered = true;

    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = rootWndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = nullptr; // LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WKEXE));
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = background_brush;
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = window_class.c_str();
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    RegisterClassEx(&wcex);
}

void RootWindow::onCanGoForwardCallback(mbWebView webView, void* param, MbAsynRequestState state, BOOL b)
{
    if (kMbAsynRequestStateOk != state)
        return;
    RootWindow* self = (RootWindow*)param;
    ::EnableWindow(self->m_forwardHwnd, b);
}

void RootWindow::onCanGoBackCallback(mbWebView webView, void* param, MbAsynRequestState state, BOOL b)
{
    if (kMbAsynRequestStateOk != state)
        return;
    RootWindow* self = (RootWindow*)param;
    ::EnableWindow(self->m_backHwnd, b);
}

void RootWindow::onUrlChangedCallback(mbWebView webView, void* param, const utf8* url, BOOL canGoBack, BOOL canGoForward)
{
    RootWindow* self = (RootWindow*)param;
    std::wstring urlString = utf8ToUtf16(url);
    ::SetWindowText(self->m_editHwnd, urlString.c_str());

    ::EnableWindow(self->m_backHwnd, canGoBack);
    ::EnableWindow(self->m_forwardHwnd, canGoForward);
}

static BOOL MB_CALL_TYPE handleLoadUrlBegin(mbWebView webView, void* param, const char* url, void* job);
static void MB_CALL_TYPE handleLoadUrlEnd(mbWebView webView, void* param, const char* url, void* job, void* buf, int len);

void RootWindow::onDocumentReady(mbWebView webView, void* param, mbWebFrameHandle frameId)
{
    if (mbIsMainFrame(webView, frameId))
        OutputDebugStringA("HandleDocumentReady main\n");
    else
        OutputDebugStringA("HandleDocumentReady not main\n");
}

void RootWindow::onLoadingFinish(mbWebView webView, void* param, mbWebFrameHandle frameId, const utf8* url, mbLoadingResult result, const utf8* failedReason)
{
    if (mbIsMainFrame(webView, frameId))
        OutputDebugStringA("handleLoadingFinish main\n");
    else
        OutputDebugStringA("handleLoadingFinish not main\n");
}

static BOOL* isRegistered = nullptr;

static void setTitle(HWND hWnd, const std::wstring& title)
{
    if (!isRegistered) {
        isRegistered = new BOOL();
        *isRegistered = mbUtilIsRegistered(L"");
    }

    std::wstring titleString = title;
    if (*isRegistered)
        titleString = L"(ÒÑ×¢²á) " + titleString;
    else
        titleString = L"(Î´×¢²á) " + titleString;

    if (titleString.size() > 35) {
        titleString = titleString.substr(0, 35);
        titleString += L"...";
    }

    ::SetWindowText(hWnd, titleString.c_str());
}

static void MB_CALL_TYPE onTitleChangedCallback(mbWebView webView, void* param, const utf8* title)
{
    RootWindow* self = (RootWindow*)param;
    std::wstring titleString = utf8ToUtf16(title);
    
    setTitle(self->getHwnd(), titleString);
}

void RootWindow::initSettings()
{
//     mbProxy proxy = { MB_PROXY_HTTP , "127.0.0.1", 1080 };
//     mbSetProxy(nullptr, &proxy);

    mbSetDragDropEnable(m_mbView, true);
    mbSetDebugConfig(m_mbView, "wakeMinInterval", "5");
    mbSetDebugConfig(m_mbView, "drawMinInterval", "300");
    mbSetDebugConfig(m_mbView, "contentScale", "100");
    mbSetCookieEnabled(m_mbView, true);
    mbSetNavigationToNewWindowEnable(m_mbView, true);

    mbSetResourceGc(m_mbView, 30);
    mbOnURLChanged(m_mbView, onUrlChangedCallback, this);
    mbOnCreateView(m_mbView, onCreateView, this);

    mbOnLoadUrlBegin(m_mbView, handleLoadUrlBegin, nullptr);
    mbOnLoadUrlEnd(m_mbView, handleLoadUrlEnd, nullptr);

    mbOnDocumentReady(m_mbView, onDocumentReady, nullptr);
    mbOnLoadingFinish(m_mbView, onLoadingFinish, nullptr);

    mbOnTitleChanged(m_mbView, onTitleChangedCallback, this);

    mbSetNodeJsEnable(m_mbView, TRUE);
}

mbWebView RootWindow::onCreateView(mbWebView parentWebviwe, void* param, mbNavigationType navType, const utf8* url, const mbWindowFeatures* features)
{
    RootWindow* rootWin = new RootWindow();
    mbWebView newWindow = rootWin->createRootWindow();
    rootWin->initSettings();

    std::wstring urlString = utf8ToUtf16(url);
    ::SetWindowText(rootWin->m_editHwnd, urlString.c_str());

    //mbSetCookieJarFullPath(newWindow, L"d:\\ÎÒcookie.txt");

    mbShowWindow(newWindow, true);
    return newWindow;
}

void writeFile(const wchar_t* path, const std::vector<char>& buffer)
{
    HANDLE hFile = CreateFileW(path, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
    if (INVALID_HANDLE_VALUE == hFile) {
        DebugBreak();
        return;
    }

    DWORD numberOfBytesWrite = 0;
    BOOL b = ::WriteFile(hFile, &buffer.at(0), buffer.size(), &numberOfBytesWrite, nullptr);
    ::CloseHandle(hFile);
    b = b;
}

void readFile(const wchar_t* path, std::vector<char>* buffer)
{
    HANDLE hFile = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (INVALID_HANDLE_VALUE == hFile) {

        std::vector<WCHAR> filenameBuffer;
        filenameBuffer.resize(MAX_PATH + 3);
        ::GetModuleFileNameW(NULL, filenameBuffer.data(), MAX_PATH);
        ::PathRemoveFileSpecW(filenameBuffer.data());

        ::PathAppendW(filenameBuffer.data(), L"mtmb.exp");
        if (::PathFileExistsW(filenameBuffer.data()))
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

static bool hookUrl(void* job, const char* url, const char* hookUrl, const wchar_t* localFile, const char* mime)
{
    if (0 == strstr(url, hookUrl))
        return false;

    //mbNetSetMIMEType(job, (char*)mime);

    std::vector<char> buffer;
    readFile(localFile, &buffer);
    if (0 == buffer.size())
        return false;

    mbNetSetData(job, &buffer[0], buffer.size());

    OutputDebugStringA("HookUrl:");
    OutputDebugStringA(url);
    OutputDebugStringA("\n");

    return true;
}

static BOOL MB_CALL_TYPE handleLoadUrlBegin(mbWebView webView, void* param, const char* url, void* job)
{
//     OutputDebugStringA("handleLoadUrlBegin:");
//     OutputDebugStringA(url);
//     OutputDebugStringA("\n"); 

//     if (hookUrl(job, url, "dore.script.js", L"E:\\test\\weibo\\dore.script.js", "text/javascript"))
//         return true;

//     if (0 != strstr(url, "video.hls.min.js")) {
//         mbNetHookRequest(job);
//     }
    
    return false;
}

static void MB_CALL_TYPE handleLoadUrlEnd(mbWebView webView, void* param, const char* url, void* job, void* buf, int len)
{

}

void RootWindow::onPaint()
{

}

void RootWindow::onSize(bool minimized)
{
    RECT rect;
    ::GetClientRect(m_hWnd, &rect);
    // Resize the window and address bar to match the new frame size.
    rect.top += urlbarHeight;

    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;

    int urloffset = rect.left + buttonWidth * 5;

    // Resize just the URL edit field.
    ::SetWindowPos(m_editHwnd, NULL, urloffset, 0, rect.right - urloffset, urlbarHeight, SWP_NOZORDER);
    mbResize(m_mbView, width, height);
    mbWake(m_mbView);
}

void RootWindow::onFocus()
{
//     HWND hWnd = mbGetHostHWND(m_mbView);
//     mbSetFocus(m_mbView);
//     ::SetCapture(hWnd);
}

void RootWindow::onWheel(WPARAM wParam, LPARAM lParam)
{
    int x = LOWORD(lParam);
    int y = HIWORD(lParam);

    unsigned int flags = 0;

    if (wParam & MK_CONTROL)
        flags |= MB_CONTROL;
    if (wParam & MK_SHIFT)
        flags |= MB_SHIFT;

    if (wParam & MK_LBUTTON)
        flags |= MB_LBUTTON;
    if (wParam & MK_MBUTTON)
        flags |= MB_MBUTTON;
    if (wParam & MK_RBUTTON)
        flags |= MB_RBUTTON;

    int delta = GET_WHEEL_DELTA_WPARAM(wParam);
    mbFireMouseWheelEvent(m_mbView, x, y, delta, flags);
}

void RootWindow::onDestroyed()
{
    setWndProcPtr(m_hWnd, nullptr);
    delete this;
}

LRESULT CALLBACK rootWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    RootWindow* self = getUserDataPtr<RootWindow*>(hWnd);
    if (!self)
        return ::DefWindowProc(hWnd, message, wParam, lParam);

    // Callback for the main window
    switch (message) {
    case WM_COMMAND:
        if (self->onCommand(LOWORD(wParam)))
            return 0;
        break;

    case WM_PAINT:
        self->onPaint();
        break;

    case WM_SIZE:
        self->onSize(wParam == SIZE_MINIMIZED);
        break;

    case WM_SETFOCUS:
        self->onFocus();
        return 0;

    case WM_KILLFOCUS:
        //mbKillFocus(self->getWkeView());
        return 0;

    case WM_MOUSEWHEEL:
        self->onWheel(wParam, lParam);
        break;
// 
//     case WM_MOVING:
//     case WM_MOVE:
//         self->OnMove();
//         return 0;
// 
    case WM_ERASEBKGND:
        // Don't erase the background.
        break;

    case WM_NCDESTROY:
        self->onDestroyed();
        return 0;
    }

    return ::DefWindowProc(hWnd, message, wParam, lParam);
}

#define MAX_URL_LENGTH 255
WNDPROC editWndProcOld = nullptr;

LRESULT CALLBACK RootWindow::editWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case WM_CHAR:
        if (wParam == VK_RETURN) {
            HWND parent = GetParent(hWnd);
            RootWindow* self = getUserDataPtr<RootWindow*>(parent);

            // When the user hits the enter key load the URL.
            std::vector<wchar_t> strPtr;
            strPtr.resize(MAX_URL_LENGTH + 1);
            *((LPWORD)&strPtr[0]) = MAX_URL_LENGTH;
            LRESULT strLen = SendMessage(hWnd, EM_GETLINE, 0, (LPARAM)&strPtr[0]);
            if (strLen > 0) {
                strPtr[strLen] = 0;

                std::string url = utf16ToUtf8(&strPtr[0]);
                mbLoadURL(self->m_mbView, url.c_str());
            }

            return 0;
        }
        if (wParam == 1 && 0x1e0001 == lParam) {
            ::PostMessage(hWnd, EM_SETSEL, 0, -1);
        }
        break;
    case WM_NCDESTROY:
        // Clear the reference to |self|.
        break;
    }

    return CallWindowProc(editWndProcOld, hWnd, message, wParam, lParam);
}

mbWebView RootWindow::createRootWindow()
{
    RECT startRect;
    startRect.left = 400;
    startRect.top = 200;
    startRect.right = startRect.left + 1080;
    startRect.bottom = startRect.top + 680;
    const DWORD dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN;

    const std::wstring& window_title = L"Miniblink";
    const std::wstring& window_class = L"MBRootWindow";

    HINSTANCE hInstance = GetModuleHandle(NULL);
    registerRootClass(hInstance, window_class, nullptr);

    int x, y, width, height;
    if (::IsRectEmpty(&startRect)) {
        // Use the default window position/size.
        x = y = width = height = CW_USEDEFAULT;
    } else {
        // Adjust the window size to account for window frame and controls.
        RECT window_rect = startRect;
        ::AdjustWindowRectEx(&window_rect, dwStyle, false, 0);

        x = startRect.left;
        y = startRect.top;
        width = window_rect.right - window_rect.left;
        height = window_rect.bottom - window_rect.top;
    }

    // Create the main window initially hidden.
    m_hWnd = CreateWindow(window_class.c_str(), window_title.c_str(),
        dwStyle,
        x, y, width, height,
        NULL, NULL, hInstance, NULL);

    setUserDataPtr(m_hWnd, this);

    createChildControl(m_hWnd);

    mbWebView mbView = mbCreateWebWindow(MB_WINDOW_TYPE_CONTROL, m_hWnd, 0, urlbarHeight, width - 8, height - urlbarHeight - 20);
    setMBView(mbView);

    initSettings();

    static bool g_firstCall = true;
    if (g_firstCall) {
        const char* url = "http://miniblink.net";
        mbLoadURL(mbView, url);
    }
    g_firstCall = false;

    setTitle(m_hWnd, L"");

    mbShowWindow(mbView, TRUE);
    ::ShowWindow(m_hWnd, SW_SHOW);
    ::UpdateWindow(m_hWnd);

    m_mbView = mbView;
    return mbView;
}

void RootWindow::createChildControl(HWND parentHwnd)
{
    HINSTANCE hInstance = GetModuleHandle(NULL);

    // Create the child controls.
    int xOffset = 0;

    RECT rect;
    ::GetClientRect(parentHwnd, &rect);

    // Create a scaled font.
    m_font = ::CreateFont(
        -fontHeight, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial");

    m_backHwnd = ::CreateWindow(
        L"BUTTON", L"Back",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_DISABLED,
        xOffset, 0, buttonWidth, urlbarHeight,
        parentHwnd, reinterpret_cast<HMENU>(IDC_NAV_BACK), hInstance, 0);
    ::SendMessage(m_backHwnd, WM_SETFONT, reinterpret_cast<WPARAM>(m_font), TRUE);
    xOffset += buttonWidth;

    m_forwardHwnd = ::CreateWindow(
        L"BUTTON", L"Forward",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_DISABLED,
        xOffset, 0, buttonWidth, urlbarHeight,
        parentHwnd, reinterpret_cast<HMENU>(IDC_NAV_FORWARD), hInstance, 0);
    ::SendMessage(m_forwardHwnd, WM_SETFONT, reinterpret_cast<WPARAM>(m_font), TRUE);
    xOffset += buttonWidth;

    m_reloadHwnd = ::CreateWindow(
        L"BUTTON", L"Reload",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON /*| WS_DISABLED*/,
        xOffset, 0, buttonWidth, urlbarHeight,
        parentHwnd, reinterpret_cast<HMENU>(IDC_NAV_RELOAD), hInstance, 0);
    ::SendMessage(m_reloadHwnd, WM_SETFONT, reinterpret_cast<WPARAM>(m_font), TRUE);
    xOffset += buttonWidth;

    m_stopHwnd = ::CreateWindow(
        L"BUTTON", L"Stop",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_DISABLED,
        xOffset, 0, buttonWidth, urlbarHeight,
        parentHwnd, reinterpret_cast<HMENU>(IDC_NAV_STOP), hInstance, 0);
    ::SendMessage(m_stopHwnd, WM_SETFONT, reinterpret_cast<WPARAM>(m_font), TRUE);
    xOffset += buttonWidth;

    m_testHwnd = ::CreateWindow(
        L"BUTTON", L"Test",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        xOffset, 0, buttonWidth, urlbarHeight,
        parentHwnd, reinterpret_cast<HMENU>(IDC_NAV_TEST), hInstance, 0);
    ::SendMessage(m_testHwnd, WM_SETFONT, reinterpret_cast<WPARAM>(m_font), TRUE);
    xOffset += buttonWidth;

    m_editHwnd = ::CreateWindow(
        L"EDIT", 0,
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOVSCROLL |
        ES_AUTOHSCROLL /*| WS_DISABLED*/,
        xOffset, 0, rect.right - buttonWidth * 5, urlbarHeight,
        parentHwnd, 0, hInstance, 0);
    ::SendMessage(m_editHwnd, WM_SETFONT, reinterpret_cast<WPARAM>(m_font), TRUE);

    // Override the edit control's window procedure.
    editWndProcOld = setWndProcPtr(m_editHwnd, editWndProc);
}
