#include "StdAfx.h"
#include <exdisp.h>
#include <comdef.h>
#include <ShellAPI.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

#include "MainWnd.h"
#include "resource.h"


void InitResource()
{
	// 资源类型
#ifdef _DEBUG
	CPaintManagerUI::SetResourceType(UILIB_FILE);
#else
	CPaintManagerUI::SetResourceType(UILIB_ZIPRESOURCE);
#endif
	// 资源路径
	CDuiString strResourcePath = CPaintManagerUI::GetInstancePath();
	// 加载资源
	switch(CPaintManagerUI::GetResourceType())
	{
	case UILIB_FILE:
		{
			strResourcePath += _T("Skin\\");
			CPaintManagerUI::SetResourcePath(strResourcePath.GetData());
			// 加载资源管理器
			CResourceManager::GetInstance()->LoadResource(_T("res.xml"), NULL);
			break;
		}
	case UILIB_RESOURCE:
		{
			strResourcePath += _T("Skin\\");
			CPaintManagerUI::SetResourcePath(strResourcePath.GetData());
			// 加载资源管理器
			CResourceManager::GetInstance()->LoadResource(_T("IDR_RES"), _T("xml"));
			break;
		}
	case UILIB_ZIP:
		{
			strResourcePath += _T("Skin\\");
			CPaintManagerUI::SetResourcePath(strResourcePath.GetData());
			CPaintManagerUI::SetResourceZip(_T("WkeBrowser.zip"), true);
			// 加载资源管理器
			CResourceManager::GetInstance()->LoadResource(_T("res.xml"), NULL);
			break;
		}
	case UILIB_ZIPRESOURCE:
		{
			strResourcePath += _T("Skin\\");
			CPaintManagerUI::SetResourcePath(strResourcePath.GetData());

			HRSRC hResource = ::FindResource(CPaintManagerUI::GetResourceDll(), _T("IDR_ZIPRES"), _T("ZIPRES"));
			if( hResource != NULL ) {
				DWORD dwSize = 0;
				HGLOBAL hGlobal = ::LoadResource(CPaintManagerUI::GetResourceDll(), hResource);
				if( hGlobal != NULL ) {
					dwSize = ::SizeofResource(CPaintManagerUI::GetResourceDll(), hResource);
					if( dwSize > 0 ) {
						CPaintManagerUI::SetResourceZip((LPBYTE)::LockResource(hGlobal), dwSize);
						// 加载资源管理器
						CResourceManager::GetInstance()->LoadResource(_T("res.xml"), NULL);
					}
				}
				::FreeResource(hResource);
			}
		}
		break;
	}
}
bool isOneInstance()
{
	HANDLE mutex = CreateMutexW(NULL, TRUE, L"Miniblink browser");
	if ((mutex != NULL) && (GetLastError() == ERROR_ALREADY_EXISTS)) {
		ReleaseMutex(mutex);
		return false;
	}
	return true;
}



int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
{

	if (!isOneInstance()) {
		::MessageBoxW(NULL, L"该进程已经启动", L"错误", MB_OK);
		return 0;
	}
	//初始化
	HRESULT Hr = ::CoInitialize(NULL);
	if( FAILED(Hr) ) return 0;
	HRESULT hRes = ::OleInitialize(NULL);
	// 初始化UI管理器
	CPaintManagerUI::SetInstance(hInstance);


	// 加载mb的资源
	CDuiString strResourcePath = CPaintManagerUI::GetInstancePath();
	CDuiString mbPath = strResourcePath + L"node.dll";
	//CDuiString mbPath = strResourcePath + L"node_v8_4_8.dll";
	if (!::PathFileExists(mbPath))
	{
		::MessageBoxW(NULL, L"请把node.dll放exe目录下", L"错误", MB_OK);
		return 0;
	}
	wkeSetWkeDllPath(mbPath);

	// 初始化资源
	InitResource();

	// 加载控件
#ifndef _DEBUG
	CPaintManagerUI::LoadPlugin(_T("TroyControls.dll"));
#else
	CPaintManagerUI::LoadPlugin(_T("TroyControls_d.dll"));
#endif


	// 创建主窗口
	CMainWnd* pMainWnd = new CMainWnd();
	if( pMainWnd == NULL ) return 0;
	pMainWnd->Create(NULL, _T("miniblink使用例子"), UI_WNDSTYLE_FRAME, 0L, 0, 0, 800, 572);
	pMainWnd->CenterWindow();

	//导入exe图标
	HICON hIcon = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDR_MAINFRAME));
	::SendMessage(pMainWnd->GetHWND(), STM_SETICON, IMAGE_ICON, (LPARAM)(UINT)hIcon);

	// 启动消息循环
	CPaintManagerUI::MessageLoop();
	// 释放资源
	CResourceManager::GetInstance()->Release();

	OleUninitialize();
	::CoUninitialize();
	return 0;
}