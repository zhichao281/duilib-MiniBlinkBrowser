#pragma once
#include <windows.h>
#include <string>
#include"../3rd/ThunderOpenSDK/xldl.h"
#include "../Common/DllLoader.h"

using namespace  WisdomUtils;

class XLDownloader 
{
public:
	 XLDownloader();
	~XLDownloader();

public:
	bool initXunLei();

	bool downloadWithXL(std::wstring url, std::wstring  filepath, std::wstring  filename);

private:
	//初始化SDK，并使其为处理后续操作做好准备。
	std::function<BOOL()> XL_Init; 

	//反初始化SDK，释放模块运行期间申请的资源
	std::function<BOOL()> XL_UnInit;

	//删除临时文件
	std::function<BOOL(DownTaskParam &stParam)> XL_DelTempFile; 

	//创建任务
	std::function<HANDLE(DownTaskParam &stParam)> XL_CreateTask;

	//销毁任务
	std::function<BOOL(HANDLE hTask)> XL_DeleteTask;

	//开始任务
	std::function<BOOL(HANDLE hTask)> XL_StartTask;

	//停止任务
	std::function<BOOL(HANDLE hTask)> XL_StopTask;

	//强制暂停任务
	std::function<BOOL(HANDLE hTask)> XL_ForceStopTask;

	//查询任务信息
	std::function<BOOL(HANDLE hTask, DownTaskInfo & stTaskInfo)> XL_QueryTaskInfoEx;

	//简单封装了XL_CreateTask接口
	std::function<HANDLE(const wchar_t *url, const wchar_t *path, const wchar_t *fileName, BOOL IsResume)> XL_CreateTaskByURL;

	//设置最大下载速度
	std::function<void(INT32 nKBps)> XL_SetSpeedLimit;

	//限制上传速度
	std::function<void(INT32 nTcpKBps, INT32 nOtherKBps)> XL_SetUploadSpeedLimit;

	//设置全局代理
	std::function<BOOL(DOWN_PROXY_INFO &stProxyInfo)> XL_SetProxy;

	//设置HTTP请求时客户端信息
	std::function<void(const wchar_t *pszUserAgent)> XL_SetUserAgent;

	//简单封装了将迅雷专用链转成实际URL	
	std::function<BOOL(const wchar_t *pszThunderUrl, wchar_t *normalUrlBuffer, INT32 bufferLen)> XL_ParseThunderPrivateUrl;


	//获取下载的文件大小
	std::function<BOOL(const wchar_t * lpURL, INT64& iFileSize)> XL_GetFileSizeWithUrl;

	std::function<BOOL(HANDLE hTask, char szFileId[40], unsigned __int64 nFileSize)> XL_SetFileIdAndSize;

	std::function<BOOL(HANDLE task_id, WSAPROTOCOL_INFOW *sock_info, CHAR *http_resp_buf, LONG buf_len)> XL_SetAdditionInfo;
	
	// 拉起迅雷7创建下载任务
	std::function<LONG(wchar_t *pszUrl, wchar_t *pszFileName, wchar_t *pszReferUrl, wchar_t *pszCharSet, wchar_t *pszCookie)> XL_CreateTaskByThunder;

	std::function<LONG(const wchar_t *pszPath)> XL_CreateBTTaskByThunder;




private :

private:
	WisdomUtils::CDllLoader  *pDllloader;

	HMODULE hDll;
};

