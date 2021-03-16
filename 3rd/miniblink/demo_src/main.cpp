/**
Copyright (c) 2018 weolar

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software. If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.

3. This notice may not be removed or altered from any source
distribution.
*/

#define _CRT_SECURE_NO_WARNINGS 

#include <Windows.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <stdbool.h>
#include <stdio.h>
#include <Commctrl.h>
#include <windowsx.h>
#include <xstring>
#include <vector>
#include "../wke.h"

#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "imm32.lib")

int APIENTRY wkeBrowserMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);

typedef struct {
    wkeWebView window;
    std::wstring url;
} Application;

Application app;

std::string utf16ToUtf8(LPCWSTR lpszSrc)
{
    std::string sResult;

    int  nUTF8Len = WideCharToMultiByte(CP_UTF8, 0, lpszSrc, -1, NULL, 0, NULL, NULL);
    char* pUTF8 = new char[nUTF8Len + 1];
    ZeroMemory(pUTF8, nUTF8Len + 1);
    WideCharToMultiByte(CP_UTF8, 0, lpszSrc, -1, pUTF8, nUTF8Len, NULL, NULL);
    sResult = pUTF8;
    delete[] pUTF8;

    return sResult;
}

std::wstring utf8ToUtf16(const std::string& utf8String)
{
    std::wstring sResult;
    int nUTF8Len = MultiByteToWideChar(CP_UTF8, 0, utf8String.c_str(), -1, NULL, NULL);
    wchar_t* pUTF8 = new wchar_t[nUTF8Len + 1];

    ZeroMemory(pUTF8, nUTF8Len + 1);
    MultiByteToWideChar(CP_UTF8, 0, utf8String.c_str(), -1, pUTF8, nUTF8Len);
    sResult = pUTF8;
    delete[] pUTF8;

    return sResult;
}

// 回调：点击了关闭、返回 true 将销毁窗口，返回 false 什么都不做。
bool handleWindowClosing(wkeWebView webWindow, void* param)
{
    Application* app = (Application*)param;
    return IDYES == MessageBoxW(NULL, L"确定要退出程序吗？", L"wkexe", MB_YESNO | MB_ICONQUESTION);
}

// 回调：窗口已销毁
void handleWindowDestroy(wkeWebView webWindow, void* param)
{
    Application* app = (Application*)param;
    app->window = NULL;
    PostQuitMessage(0);
}

// 回调：文档加载成功
void handleDocumentReady(wkeWebView webWindow, void* param)
{
    wkeShowWindow(webWindow, true);
}

// 回调：页面标题改变
void handleTitleChanged(wkeWebView webWindow, void* param, const wkeString title)
{
    wkeSetWindowTitleW(webWindow, wkeGetStringW(title));
}

// 回调：创建新的页面，比如说调用了 window.open 或者点击了 <a target="_blank" .../>
wkeWebView onCreateView(wkeWebView webWindow, void* param, wkeNavigationType navType, const wkeString url, const wkeWindowFeatures* features)
{
    wkeWebView newWindow = wkeCreateWebWindow(WKE_WINDOW_TYPE_POPUP, NULL, features->x, features->y, features->width, features->height);
    wkeShowWindow(newWindow, true);
    return newWindow;
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

std::wstring getResourcesPath(const std::wstring& name)
{
    std::wstring result;
    std::wstring temp;
    std::vector<wchar_t> path;
    path.resize(MAX_PATH + 1);
    ::GetModuleFileName(nullptr, &path[0], MAX_PATH);

    ::PathRemoveFileSpecW(&path[0]);
    temp += &path[0];
    temp += L"\\";

    result = temp + name;
    if (!::PathFileExists(result.c_str())) {
        result = temp + L"..\\";
        result += name;
    }
    return result;
}

const char kPreHead[] = "http://hook.test/";

bool onLoadUrlBegin(wkeWebView webView, void* param, const char* url, void *job)
{
    const char* pos = strstr(url, kPreHead);
    if (pos) {
        const utf8* decodeURL = wkeUtilDecodeURLEscape(url);
        if (!decodeURL)
            return false;
        std::string urlString(decodeURL);
        std::string localPath = urlString.substr(sizeof(kPreHead) - 1);

        std::wstring path = getResourcesPath(utf8ToUtf16(localPath));
        std::vector<char> buffer;

        readJsFile(path.c_str(), &buffer);

        wkeNetSetData(job, buffer.data(), buffer.size());

        return true;
    } else if (strncmp(url, "http://localhost:12222", 22) == 0) {
        wkeNetSetMIMEType(job, (char*)"text/html");
        wkeNetSetData(job, (char*)"\"test1111\"", 10);
        return true;
    } else if (strcmp(url, "http://www.baidu.com/") == 0) {
        wkeNetHookRequest(job);
    }
    return false;
}

void onLoadUrlEnd(wkeWebView webView, void* param, const char *url, void *job, void* buf, int len)
{
    const wchar_t *str = L"百度一下";
    const wchar_t *str1 = L"HOOK一下";

    int slen = ::WideCharToMultiByte(CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL);
    if (slen == 0) return;

    char utf81[100];
    ::WideCharToMultiByte(CP_UTF8, 0, str, -1, &utf81[0], slen, NULL, NULL);

    slen = ::WideCharToMultiByte(CP_UTF8, 0, str1, -1, NULL, 0, NULL, NULL);
    if (slen == 0) return;

    char utf82[100];
    ::WideCharToMultiByte(CP_UTF8, 0, str1, -1, &utf82[0], slen, NULL, NULL);

    const char *b = strstr(static_cast<const char*>(buf), &utf81[0]);
    memcpy((void *)b, &utf82, slen);
    return;
}

void blinkMaximize()
{
    HWND hwnd = wkeGetWindowHandle(app.window);
    static bool isMax = true; 
    if (isMax)
        ::PostMessageW(hwnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
    else 
        ::PostMessageW(hwnd, WM_SYSCOMMAND, SC_RESTORE, 0);
    isMax = !isMax;
}

void blinkMinimize()
{
    HWND hwnd = wkeGetWindowHandle(app.window);
    ::PostMessageW(hwnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
}

void blinkClose()
{
    HWND hwnd = wkeGetWindowHandle(app.window);
    ::PostMessageW(hwnd, WM_CLOSE, 0, 0);
}

HWND getHWND()
{
    return wkeGetWindowHandle(app.window);
}

// 互相调用：CPP调用JS
const char* callJSFunc(jsExecState es, char *funcName, char *param)
{
    jsValue f = jsGetGlobal(es, funcName);
    jsValue val = jsString(es, param);
    jsValue callRet = jsCallGlobal(es, f, &val, 1);
    return jsToString(es, callRet);
}

// 互相调用：JS调用CPP
jsValue JS_CALL js_msgBox(jsExecState es)
{
    const wchar_t* text = jsToStringW(es, jsArg(es, 0));
    const wchar_t* title = jsToStringW(es, jsArg(es, 1));
    ::MessageBoxW(NULL, text, title, 0);
    return jsStringW(es, L"C++返回字符串");
}

// 以下设置窗口拖动区域
static bool g_isMoving = false;
static RECT g_moveRect = { 0 };

void setMoveWindowArea(int x, int y, int w, int h)
{
    g_moveRect.left = x;
    g_moveRect.top = y;
    g_moveRect.right = x + w;
    g_moveRect.bottom = y + h;
    g_isMoving = true;
}

LRESULT CALLBACK subClassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
//     if (g_isMoving && (uMsg == WM_NCHITTEST)) {
//         POINT pt = { (LONG)GET_X_LPARAM(lParam), (LONG)GET_Y_LPARAM(lParam) };
//         ::ScreenToClient(hWnd, &pt);
//         if (::PtInRect(&g_moveRect, pt))
//             return HTCAPTION;
//         
//     }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

jsValue onMsg(jsExecState es, void* param)
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

    if ("close" == msg) {
        blinkClose();
    } else if ("max" == msg) {
        blinkMaximize();
    } else if ("min" == msg) {
        blinkMinimize();
    }

    return jsUndefined();
}

bool createECharsTest()
{
    wkeWebView webview = wkeCreateWebWindow(WKE_WINDOW_TYPE_TRANSPARENT, NULL, 0, 0, 640, 480);
    wkeSetWindowTitleW(webview, L"miniblink ECharsTest");
    wkeOnDocumentReady(webview, handleDocumentReady, nullptr);
    wkeOnLoadUrlBegin(webview, onLoadUrlBegin, nullptr);
    wkeOnLoadUrlEnd(webview, onLoadUrlEnd, nullptr);
    wkeMoveToCenter(webview);
    wkeLoadURL(webview, "http://hook.test/resources/view/map.html");

    return true;
}

jsValue WKE_CALL_TYPE onShellExec(jsExecState es, void* param)
{
    if (0 == jsArgCount(es))
        return jsUndefined();
    jsValue arg0 = jsArg(es, 0);
    if (!jsIsString(arg0))
        return jsUndefined();

    std::string path;
    path = jsToTempString(es, arg0);
    if ("runEchars" == path) {
        createECharsTest();
    } else if ("wkeBrowser" == path) {
        wkeBrowserMain(nullptr, nullptr, nullptr, TRUE);
    }

    path += "\n";
    OutputDebugStringA(path.c_str());

    return jsUndefined();
}

void WKE_CALL_TYPE onDidCreateScriptContextCallback(wkeWebView webView, void* param, wkeWebFrameHandle frameId, void* context, int extensionGroup, int worldId)
{
    
}

bool createWebWindow(Application* app)
{
    app->window = wkeCreateWebWindow(WKE_WINDOW_TYPE_TRANSPARENT, NULL, 0, 0, 640, 480); 
    if (!app->window)
        return false;

    //SetWindowSubclass(wkeGetWindowHandle(app->window), subClassProc, 0, 0);
    setMoveWindowArea(0, 0, 640, 30); // 设置窗口可拖动区域，用于无边框窗体

    wkeSetWindowTitleW(app->window, L"miniblink demo");

    wkeOnDidCreateScriptContext(app->window, onDidCreateScriptContextCallback, app);
    wkeOnWindowClosing(app->window, handleWindowClosing, app);
    wkeOnWindowDestroy(app->window, handleWindowDestroy, app);
    wkeOnDocumentReady(app->window, handleDocumentReady, app);
    wkeOnTitleChanged(app->window, handleTitleChanged, app);
    wkeOnCreateView(app->window, onCreateView, app);
    wkeOnLoadUrlBegin(app->window, onLoadUrlBegin, app);
    wkeOnLoadUrlEnd(app->window, onLoadUrlEnd, app);
    wkeSetDebugConfig(app->window, "decodeUrlRequest", nullptr);

    wkeJsBindFunction("eMsg", &onMsg, nullptr, 5);
    wkeJsBindFunction("eShellExec", &onShellExec, nullptr, 3);
    wkeMoveToCenter(app->window);
    wkeLoadURLW(app->window, app->url.c_str());

    return true;
}

void runMessageLoop(Application* app)
{
    MSG msg = { 0 };
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

void quitApplication(Application* app)
{
    if (app->window) {
        wkeDestroyWebWindow(app->window);
        app->window = NULL;
    }
}

void runApp(Application* app)
{
    memset(app, 0, sizeof(Application));
    app->url = L"http://hook.test/resources/view/index.html"; // 演示使用hook的方式加载资源
    if (!createWebWindow(app)) {
        PostQuitMessage(0);
        return;
    }
    runMessageLoop(app);
}

bool isOneInstance()
{
    HANDLE mutex = CreateMutexW(NULL, TRUE, L"Hello Miniblink");
    if ((mutex != NULL) && (GetLastError() == ERROR_ALREADY_EXISTS)) {
        ReleaseMutex(mutex);
        return false;
    }
    return true;
}

int WINAPI wWinMain(
    __in HINSTANCE hInstance,
    __in_opt HINSTANCE hPrevInstance,
    __in LPWSTR lpCmdLine,
    __in int nCmdShow
    )
{
    if (!isOneInstance()) {
        ::MessageBoxW(NULL, L"该进程已经启动", L"错误", MB_OK);
        return 0;
    }
    
    std::vector<wchar_t> tempPath;
    tempPath.resize(MAX_PATH);

    ::GetModuleFileNameW(nullptr, &tempPath[0], MAX_PATH);
    ::PathRemoveFileSpec(&tempPath[0]);

    std::vector<wchar_t> mbPath = tempPath;

    ::PathAppendW(&mbPath[0], L"node.dll");
    if (!::PathFileExists(&mbPath[0])) {
        ::PathAppendW(&tempPath[0], L"..\\..\\");
        mbPath = tempPath;
        ::PathAppendW(&mbPath[0], L"node.dll");
        if (!::PathFileExists(&mbPath[0])) {
            ::MessageBoxW(NULL, L"请把node.dll放exe目录下", L"错误", MB_OK);
            return 0;
        }
    }

    wkeSetWkeDllPath(&mbPath[0]);
    wkeInitialize();
    runApp(&app);
    wkeFinalize();
    return 0;
}
