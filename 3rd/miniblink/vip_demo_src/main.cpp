#define _CRT_SECURE_NO_WARNINGS 1

#include "../mb.h"
#include "RootWindow.h"
#include <windows.h>
#include <string>
#include <vector>
#include <time.h>
#include <shlwapi.h>

#define kClassWindow L"TestMbWindow"

LRESULT WINAPI testWindowProc(
    __in HWND hWnd,
    __in UINT msg,
    __in WPARAM wParam,
    __in LPARAM lParam)
{
    LRESULT result = 0;
    mbWebView view = (mbWebView)::GetProp(hWnd, L"mb");
    if (!view)
        return ::DefWindowProc(hWnd, msg, wParam, lParam);

    switch (msg) {
    case WM_NCDESTROY:
        if (::GetProp(hWnd, L"subView")) {
            RemoveProp(hWnd, L"subView");
        } else {
            mbDestroyWebView(view);
        }
        return 0;

    case WM_ERASEBKGND:
        return TRUE;

    case WM_PAINT:
    {
        if (WS_EX_LAYERED == (WS_EX_LAYERED & GetWindowLong(hWnd, GWL_EXSTYLE)))
            break;
        //mbRepaintIfNeeded(view);

        PAINTSTRUCT ps = { 0 };
        HDC hdc = ::BeginPaint(hWnd, &ps);

        RECT rcClip = ps.rcPaint;

        RECT rcClient;
        ::GetClientRect(hWnd, &rcClient);

        RECT rcInvalid = rcClient;
        if (rcClip.right != rcClip.left && rcClip.bottom != rcClip.top)
            ::IntersectRect(&rcInvalid, &rcClip, &rcClient);

        int srcX = rcInvalid.left - rcClient.left;
        int srcY = rcInvalid.top - rcClient.top;
        int destX = rcInvalid.left;
        int destY = rcInvalid.top;
        int width = rcInvalid.right - rcInvalid.left;
        int height = rcInvalid.bottom - rcInvalid.top;

        if (0 != width && 0 != height) {
            HDC hMbDC = mbGetLockedViewDC(view);
            ::BitBlt(hdc, destX, destY, width, height, hMbDC, srcX, srcY, SRCCOPY);
            mbUnlockViewDC(view);
        }

        ::EndPaint(hWnd, &ps);
        return 1;
        break;
    }
    case WM_SIZE:
    {
        RECT rc = { 0 };
        ::GetClientRect(hWnd, &rc);
        int width = rc.right - rc.left;
        int height = rc.bottom - rc.top;

        ::mbResize(view, width, height);
        // mbRepaintIfNeeded(view);
        ::mbWake(view);

        return 0;
    }
    case WM_KEYDOWN:
    {
        unsigned int virtualKeyCode = wParam;
        unsigned int flags = 0;
        if (HIWORD(lParam) & KF_REPEAT)
            flags |= MB_REPEAT;
        if (HIWORD(lParam) & KF_EXTENDED)
            flags |= MB_EXTENDED;

        if (mbFireKeyDownEvent(view, virtualKeyCode, flags, false))
            return 0;
        break;
    }
    case WM_KEYUP:
    {
        unsigned int virtualKeyCode = wParam;
        unsigned int flags = 0;
        if (HIWORD(lParam) & KF_REPEAT)
            flags |= MB_REPEAT;
        if (HIWORD(lParam) & KF_EXTENDED)
            flags |= MB_EXTENDED;

        if (mbFireKeyUpEvent(view, virtualKeyCode, flags, false))
            return 0;
        break;
    }
    case WM_CHAR:
    {
        unsigned int charCode = wParam;
        unsigned int flags = 0;
        if (HIWORD(lParam) & KF_REPEAT)
            flags |= MB_REPEAT;
        if (HIWORD(lParam) & KF_EXTENDED)
            flags |= MB_EXTENDED;

        if (mbFireKeyPressEvent(view, charCode, flags, false))
            return 0;
        break;
    }
    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_LBUTTONDBLCLK:
    case WM_MBUTTONDBLCLK:
    case WM_RBUTTONDBLCLK:
    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MOUSEMOVE:
    {
        if (msg == WM_LBUTTONDOWN || msg == WM_MBUTTONDOWN || msg == WM_RBUTTONDOWN) {
            if (::GetFocus() != hWnd)
                ::SetFocus(hWnd);
            ::SetCapture(hWnd);
        } else if (msg == WM_LBUTTONUP || msg == WM_MBUTTONUP || msg == WM_RBUTTONUP) {
            ReleaseCapture();
        }

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

        if (mbFireMouseEvent(view, msg, x, y, flags))
            return 0;
        break;
    }
    case WM_CONTEXTMENU:
    {
        POINT pt;
        pt.x = LOWORD(lParam);
        pt.y = HIWORD(lParam);

        if (pt.x != -1 && pt.y != -1)
            ScreenToClient(hWnd, &pt);

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

        if (mbFireContextMenuEvent(view, pt.x, pt.y, flags))
            return 0;
        break;
    }
    case WM_MOUSEWHEEL:
    {
        POINT pt;
        pt.x = LOWORD(lParam);
        pt.y = HIWORD(lParam);
        ::ScreenToClient(hWnd, &pt);

        int delta = GET_WHEEL_DELTA_WPARAM(wParam);

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

        if (mbFireMouseWheelEvent(view, pt.x, pt.y, delta, flags))
            return 0;
        break;
    }
    case WM_SETFOCUS:
        mbSetFocus(view);
        return 0;

    case WM_KILLFOCUS:
        mbKillFocus(view);
        return 0;

    case WM_SETCURSOR:
        if (mbFireWindowsMessage(view, hWnd, WM_SETCURSOR, 0, 0, &result))
            return result;
        break;

    case WM_IME_STARTCOMPOSITION: {
        if (mbFireWindowsMessage(view, hWnd, WM_IME_STARTCOMPOSITION, 0, 0, &result))
            return result;
        break;
        }
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

BOOL regWndClass(LPCTSTR lpcsClassName, DWORD dwStyle)
{
    WNDCLASS wndclass = { 0 };

    wndclass.style = dwStyle;
    wndclass.lpfnWndProc = testWindowProc;
    wndclass.cbClsExtra = 200;
    wndclass.cbWndExtra = 200;
    wndclass.hInstance = ::GetModuleHandle(NULL);
    wndclass.hIcon = NULL;
    //wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = lpcsClassName;

    ::RegisterClass(&wndclass);
    return TRUE;
}

BOOL unregWndClass(LPCTSTR lpcsClassName)
{
    ::UnregisterClass(lpcsClassName, ::GetModuleHandle(NULL));
    return TRUE;
}

void MB_CALL_TYPE handlePaintUpdatedCallback(mbWebView webView, void* param, const HDC hdc, int x, int y, int cx, int cy)
{
    HWND hWnd = (HWND)param;
    BOOL callOk = FALSE;
    if (WS_EX_LAYERED == (WS_EX_LAYERED & GetWindowLong(hWnd, GWL_EXSTYLE))) {
        RECT rectDest;
        ::GetWindowRect(hWnd, &rectDest);

        SIZE sizeDest = { rectDest.right - rectDest.left, rectDest.bottom - rectDest.top };
        POINT pointDest = { 0, 0 }; // { rectDest.left, rectDest.top };
        POINT pointSource = { 0, 0 };

        BITMAP bmp = { 0 };
        HBITMAP hBmp = (HBITMAP)::GetCurrentObject(hdc, OBJ_BITMAP);
        ::GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp);

        sizeDest.cx = bmp.bmWidth;
        sizeDest.cy = bmp.bmHeight;

        HDC hdcScreen = GetDC(hWnd);

        BLENDFUNCTION blend = { 0 };
        blend.BlendOp = AC_SRC_OVER;
        blend.SourceConstantAlpha = 255;
        blend.AlphaFormat = AC_SRC_ALPHA;
        callOk = ::UpdateLayeredWindow(hWnd, hdcScreen, nullptr, &sizeDest, hdc, &pointSource, RGB(0xFF, 0xFF, 0xFF), &blend, ULW_ALPHA);
        if (!callOk) {
            HDC hdcMemory = ::CreateCompatibleDC(hdcScreen);
            HBITMAP hbmpMemory = ::CreateCompatibleBitmap(hdcScreen, sizeDest.cx, sizeDest.cy);
            HBITMAP hbmpOld = (HBITMAP)::SelectObject(hdcMemory, hbmpMemory);

            ::BitBlt(hdcMemory, 0, 0, sizeDest.cx, sizeDest.cy, hdc, 0, 0, SRCCOPY | CAPTUREBLT);

            ::BitBlt(hdc, 0, 0, sizeDest.cx, sizeDest.cy, hdcMemory, 0, 0, SRCCOPY | CAPTUREBLT); //!

            callOk = ::UpdateLayeredWindow(hWnd, hdcScreen, nullptr, &sizeDest, hdcMemory, &pointSource, RGB(0xFF, 0xFF, 0xFF), &blend, ULW_ALPHA);

            ::SelectObject(hdcMemory, (HGDIOBJ)hbmpOld);
            ::DeleteObject((HGDIOBJ)hbmpMemory);
            ::DeleteDC(hdcMemory);
        }

        ::ReleaseDC(hWnd, hdcScreen);
    } else {
        RECT rc = { x, y, x + cx, y + cy };
        ::InvalidateRect(hWnd, &rc, FALSE);
    }
}

void MB_CALL_TYPE onRunJs(mbWebView webView, void* param, mbJsExecState es, mbJsValue v)
{
    const utf8* str = mbJsToString(es, v);

    OutputDebugStringA("onRunJs:");
    OutputDebugStringA(str);
    OutputDebugStringA("\n");
}

void MB_CALL_TYPE handleDocumentReady(mbWebView webView, void* param, mbWebFrameHandle frameId)
{
    OutputDebugStringA("HandleDocumentReady\n");

    mbRunJs(webView, mbWebFrameGetMainFrame(webView), "return window.onNativeRunjs('I am runjs');", TRUE, onRunJs, nullptr, nullptr);
}

void MB_CALL_TYPE handleLoadingFinish(mbWebView webView, void* param, mbWebFrameHandle frameId, const utf8* url, mbLoadingResult result, const utf8* failedReason)
{
    //if(result == MB_LOADING_SUCCEEDED)
    //::mbNetGetFavicon(webView, HandleFaviconReceived, param);
    OutputDebugStringA("handleLoadingFinish \n");
}

mbWebView MB_CALL_TYPE handleCreateView(mbWebView webView, void* param, mbNavigationType navigationType, const utf8* url, const mbWindowFeatures* windowFeatures)
{
    mbWebView view = mbCreateWebView();
    HWND hWnd = ::CreateWindowEx(WS_EX_APPWINDOW, kClassWindow, NULL, WS_OVERLAPPEDWINDOW | WS_VISIBLE, windowFeatures->x, windowFeatures->y, windowFeatures->width, windowFeatures->height, NULL, NULL, ::GetModuleHandle(NULL), NULL);
    ::SetProp(hWnd, L"mb", (HANDLE)view);
    ::SetProp(hWnd, L"subView", (HANDLE)TRUE);
    ::mbSetHandle(view, hWnd);
    ::mbOnPaintUpdated(view, handlePaintUpdatedCallback, hWnd);
    ::mbOnLoadingFinish(view, handleLoadingFinish, view);
    ::mbOnCreateView(view, handleCreateView, view);
    ::mbSetNavigationToNewWindowEnable(view, true);
    ::mbSetCspCheckEnable(view, false);

    RECT rc = { 0 };
    ::GetClientRect(hWnd, &rc);
    ::mbResize(view, rc.right, rc.bottom);

    //mbShowWindow(view, TRUE);
    return view;
}

void MB_CALL_TYPE onJsQuery(mbWebView webView, void* param, mbJsExecState es, int64_t queryId, int customMsg, const utf8* request)
{
    OutputDebugStringA("onJsQuery:");
    OutputDebugStringA(request);
    OutputDebugStringA("\n");

    mbResponseQuery(webView, queryId, customMsg, "I am response");
}

void createSimpleMb()
{
    regWndClass(kClassWindow, CS_HREDRAW | CS_VREDRAW);
    mbWebView view = mbCreateWebView();
    HWND hWnd = ::CreateWindowEx(WS_EX_APPWINDOW, kClassWindow, NULL, WS_OVERLAPPEDWINDOW, 0, 0, 840, 680, NULL, NULL, ::GetModuleHandle(NULL), NULL);
    ::SetProp(hWnd, L"mb", (HANDLE)view);
    mbSetHandle(view, hWnd);
    mbOnPaintUpdated(view, handlePaintUpdatedCallback, hWnd);
    //::mbOnLoadUrlBegin(view, handleLoadUrlBegin, view);
    mbOnDocumentReady(view, handleDocumentReady, view);
    mbOnLoadingFinish(view, handleLoadingFinish, view);
    mbOnCreateView(view, handleCreateView, view);
    mbSetNavigationToNewWindowEnable(view, true);
    mbSetCspCheckEnable(view, false);
    mbMoveToCenter(view);
    
    mbLoadHtmlWithBaseUrl(view,
        "<html><head><style></style><script type=\"text/javascript\">"
        "window.onNativeRunjs = function(response) {"
        "    console.log('onNativeRunjs:' + response);"
        "    return 'onNativeRunjs ret'"
        "};"
        "function onNativeResponse(customMsg, response) {"
        "    console.log('mbQuery:' + response);"
        "};"
        "console.log('test');"
        "window.mbQuery(0x123456, \"I am in js context\", onNativeResponse);"
        "</script></head>"
        "<body>"
        "test js bind"
        ""
        "</body>"
        "</html>",
        "test_js.htm");
    
    //::mbLoadURL(view, "http://news.baidu.com");

    mbOnJsQuery(view, onJsQuery, (void*)1);

    mbShowWindow(view, TRUE);
}

void createMbClient()
{
    RootWindow* rootWin = new RootWindow();
    rootWin->createRootWindow();
}

void initDllPath()
{
    std::vector<wchar_t> path;
    path.resize(MAX_PATH + 1);
    memset(&path.at(0), 0, sizeof(wchar_t) * (MAX_PATH + 1));

    ::GetModuleFileNameW(nullptr, &path.at(0), MAX_PATH);
    ::PathRemoveFileSpecW(&path.at(0));

    std::vector<wchar_t>* mbPath = new std::vector<wchar_t>(path);
    std::vector<wchar_t>* mbMainPath = new std::vector<wchar_t>(path);
    ::PathAppendW(&mbPath->at(0), L"mb.dll");

    if (::PathFileExistsW(&mbPath->at(0))) {
        mbSetMbDllPath(&mbPath->at(0));

        ::PathAppendW(&mbMainPath->at(0), L"node.dll");
        mbSetMbMainDllPath(&mbMainPath->at(0));
    } else {
        delete mbPath;
        mbPath = new std::vector<wchar_t>(path);

        ::PathAppendW(&mbPath->at(0), L"..\\..\\mb.dll");
        mbSetMbDllPath(&mbPath->at(0));

        ::PathAppendW(&mbMainPath->at(0), L"..\\..\\node.dll");
        mbSetMbMainDllPath(&mbMainPath->at(0));
    }
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    initDllPath();

    mbSettings settings;
    memset(&settings, 0, sizeof(settings));
    settings.mask = MB_ENABLE_NODEJS;
    mbInit(&settings);

    createMbClient();
    createSimpleMb();

    MSG msg = { 0 };
    while (true) {
        if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (WM_QUIT == msg.message)
                break;
            ::TranslateMessage(&msg);
            ::DispatchMessageW(&msg);
        }
        ::mbWake(nullptr);
        ::Sleep(5);
    }

    mbUninit();

    return 0;
}