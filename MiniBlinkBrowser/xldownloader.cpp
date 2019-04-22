#include "xldownloader.h"
#include <stdio.h>
#include <windows.h> 
#include <TCHAR.H>
XLDownloader::XLDownloader()
{
	m_QueryTaskInfoThread = nullptr;
	m_pCallbackParam = nullptr;
	m_bRun.store(TRUE);
}


XLDownloader::~XLDownloader()
{
	m_bRun.store(FALSE);
	if (m_QueryTaskInfoThread != nullptr)
	{
		m_QueryTaskInfoThread->join();
	}
	XL_UnInit();
	
}


BOOL GetFilePathInModule(TCHAR *pFilePath, TCHAR *pFileName, HMODULE hMod)
{
	TCHAR	szModPath[MAX_PATH];
	TCHAR*	pFind;

	GetModuleFileName(hMod, szModPath, MAX_PATH);
	pFind = _tcsrchr(szModPath, _T('\\'));
	if (pFind == NULL)
	{
		pFilePath[0] = 0;
		return FALSE;
	}

	_tcscpy_s(pFilePath, MAX_PATH, szModPath);
	if (pFileName)
	{
		pFilePath[pFind - szModPath + 1] = 0;
		_tcscat_s(pFilePath, MAX_PATH, pFileName);
	}

	return TRUE;
}
bool XLDownloader::initXunLei()
{
	if (pDllloader!=nullptr)
	{
		return false;
	}
	TCHAR modulePath[MAX_PATH] = { 0 };
	TCHAR dlldir[MAX_PATH] = { 0 };
	GetFilePathInModule(dlldir,_T("xldl.dll"),NULL);
	pDllloader = new CDllLoader(dlldir);   //声明所用到的dll文件	
	if (pDllloader == nullptr)
	{
		::MessageBoxW(NULL, L"请把xldl.dll放exe目录下", L"错误", MB_OK);
		return false;
	}
	if (pDllloader->LoadResult())
	{
		XL_Init = pDllloader->GetFunction<BOOL()>("XL_Init");
		bool retflag = XL_Init();                      //调用迅雷程序初始化程序
		if (retflag == false)                       //返回fasle，则迅雷初始化失败
		{
			return false;
		}
		else
		{    //初始化成功 

			XL_Init = pDllloader->GetFunction<BOOL()>("XL_Init");

			XL_UnInit = pDllloader->GetFunction<BOOL()>("XL_UnInit");

			XL_DelTempFile = pDllloader->GetFunction<BOOL(DownTaskParam &stParam)>("XL_DelTempFile");

			XL_CreateTask = pDllloader->GetFunction<HANDLE(DownTaskParam &stParam)>("XL_CreateTask");

			XL_DeleteTask = pDllloader->GetFunction<BOOL(HANDLE)>("XL_DeleteTask");

			XL_StartTask = pDllloader->GetFunction<BOOL(HANDLE)>("XL_StartTask");

			XL_StopTask = pDllloader->GetFunction<BOOL(HANDLE)>("XL_StopTask");

			XL_ForceStopTask = pDllloader->GetFunction<BOOL(HANDLE)>("XL_ForceStopTask");

			XL_QueryTaskInfoEx = pDllloader->GetFunction<BOOL(HANDLE hTask, DownTaskInfo & stTaskInfo)>("XL_QueryTaskInfoEx");

			XL_CreateTaskByURL = pDllloader->GetFunction<HANDLE(const wchar_t *url, const wchar_t *path, const wchar_t *fileName, BOOL IsResume)>("XL_CreateTaskByURL");

			XL_SetSpeedLimit = pDllloader->GetFunction<void(INT32 nKBps)>("XL_SetSpeedLimit");

			XL_SetUploadSpeedLimit = pDllloader->GetFunction<void(INT32 nTcpKBps, INT32 nOtherKBps)>("XL_SetUploadSpeedLimit");

			XL_SetProxy = pDllloader->GetFunction<BOOL(DOWN_PROXY_INFO &stProxyInfo)>("XL_SetProxy");

			XL_SetUserAgent = pDllloader->GetFunction<void(const wchar_t *pszUserAgent)>("XL_SetUserAgent");

			XL_ParseThunderPrivateUrl = pDllloader->GetFunction<BOOL(const wchar_t *pszThunderUrl, wchar_t *normalUrlBuffer, INT32 bufferLen)>("XL_ParseThunderPrivateUrl");

			XL_GetFileSizeWithUrl = pDllloader->GetFunction< BOOL(const wchar_t * lpURL, INT64& iFileSize)>("XL_GetFileSizeWithUrl");
			
			XL_SetFileIdAndSize = pDllloader->GetFunction<BOOL(HANDLE hTask, char szFileId[40], unsigned __int64 nFileSize)>("XL_SetFileIdAndSize");
		
			XL_SetAdditionInfo = pDllloader->GetFunction<BOOL(HANDLE task_id, WSAPROTOCOL_INFOW *sock_info, CHAR *http_resp_buf, LONG buf_len)>("XL_SetAdditionInfo");

			XL_CreateTaskByThunder = pDllloader->GetFunction<LONG(wchar_t *pszUrl, wchar_t *pszFileName, wchar_t *pszReferUrl, wchar_t *pszCharSet, wchar_t *pszCookie)>("XL_CreateTaskByThunder");
		
			XL_CreateBTTaskByThunder = pDllloader->GetFunction<LONG(const wchar_t *pszPath)>("XL_CreateBTTaskByThunder");


			return true;
		}
	}
	
}
bool XLDownloader::downloadWithXL(std::wstring url, std::wstring  filepath,std::wstring  filename)
{
	HANDLE  hDownTask = XL_CreateTaskByURL(url.c_str(), filepath.c_str(), filename.c_str(), FALSE);
	bool bres=XL_StartTask(hDownTask);
	m_vecTask.push_back(hDownTask);
	if (m_QueryTaskInfoThread == nullptr)
	{
		m_QueryTaskInfoThread = new std::thread([this]()
		{
			while (m_bRun.load())
			{
				for (HANDLE hDownTask : m_vecTask)
				{
					DownTaskInfo  stTaskInfo;
				    BOOL bRet=	XL_QueryTaskInfoEx(hDownTask, stTaskInfo);
					if (m_callbackfuc != nullptr && bRet)
					{
						m_callbackfuc(m_pCallbackParam,hDownTask, stTaskInfo);
					}				
				}
				Sleep(1000);



			}
		});
		m_QueryTaskInfoThread->detach();
	}


	return bres;


}

void XLDownloader::setCallback(CallBackFunc callbackfuc, void * lpParam)
{
	m_callbackfuc = callbackfuc;

	m_pCallbackParam = lpParam;
}
