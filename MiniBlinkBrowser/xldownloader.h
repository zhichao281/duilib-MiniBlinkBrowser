#pragma once

#include <windows.h>
#include <string>
#include <thread>
#include <atomic>
#include <vector>
#include"../3rd/ThunderOpenSDK/xldl.h"
#include "../Common/DllLoader.h"

using namespace  WisdomUtils;


typedef std::function<void(void* lpParam, HANDLE hTask, DownTaskInfo stTaskInfo)>  CallBackFunc;
class XLDownloader 
{
public:
	 XLDownloader();
	~XLDownloader();

public:
	bool initXunLei();

	bool downloadWithXL(std::wstring url, std::wstring  filepath, std::wstring  filename);

	//设置回调
	void  setCallback(CallBackFunc callbackfuc, void* lpParam = nullptr);

private:

	//初始化下载引擎，并使其为处理后续操作做好准备。
	//返回值：TRUE表示成功，FALSE表示失败。一些特殊情况会导致初始化失败，如：二进制文件缺失。
	//说明：该接口必须与XL_Unint(void)成对出现。调用该接口的时，xldl.dll会拉起MiniThunderPlatform.exe子进程。
	std::function<BOOL()> XL_Init; 

	//反初始化下载引擎，释放模块运行期间申请的资源。
	//返回值：TRUE表示成功，FALSE表示失败。
	//说明：调用该接口后，会通知MiniThunderPlatform.exe子进程退出。
	std::function<BOOL()> XL_UnInit;

	//创建任务。可以新建任务也可以续传任务，参数中IsResume用于表示任务是否是续传任务。其他信息请参考DownTaskParam的定义。
	//参数：[in] stParam，创建任务需要的参数。
	//DownTaskParam参考：
	//struct DownTaskParam {
	//int nReserved1; TCHAR szTaskUrl[2084]; // 任务URL，支持http,https,ftp,ftps格式 TCHAR szRefUrl[2084]; // 可空，HTTP下载所在的网页URL TCHAR szCookies[4096]; // 可空，浏览器cookie TCHAR szFilename[MAX_PATH]; // 下载保存文件名. TCHAR szReserved[MAX_PATH]; TCHAR szSavePath[MAX_PATH]; // 文件保存目录 HWND hReserved; BOOL bReserved1; TCHAR szReserved1[64]; TCHAR szReserved2[64]; BOOL IsOnlyOriginal; //是否只从原始地址下载 UINT nReserved2; BOOL bReserved2; BOOL IsResume; // 是否用续传 DWORD reserved[2048]; }
	//返回值：返回任务的句柄。
	//说明：任务创建后，是不会马上就下载的，需要调用XL_StartTask。异步执行。
	std::function<HANDLE(DownTaskParam &stParam)> XL_CreateTask;

	//删除临时文件
	std::function<BOOL(DownTaskParam &stParam)> XL_DelTempFile; 


	//开始下载。任务创建成功后，不会马上开始下载，需调用此接口才会开始下载。
	//参数：[in]hTask，任务句柄
	//返回值：TRUE表示成功，FALSE表示失败。当任务不存在时，调用会失败。
	//说明：异步执行, 如果任务出于pending状态则不能调用该接口。
	std::function<BOOL(HANDLE hTask)> XL_StartTask;

	//销毁任务，释放任务在运行期间申请的资源。
	//参数：[in]hTask，任务句柄
	//返回值：TRUE表示成功，FALSE表示失败。当任务不存在时，调用会失败。
	//说明：在任务暂停、成功以及删除任务的时候都需要调用该接口，保证资源尽快的释放。 
	//异步执行, 如果任务出于pending状态则不能调用该接口。
	std::function<BOOL(HANDLE hTask)> XL_DeleteTask;

	//停止下载
	//参数：[in]hTask，任务句柄
	//返回值：TRUE表示成功，FALSE表示失败。当任务不存在时，调用会失败。
	//说明：获取任务信息是使用轮询方式的，最终任务下载了多少数据在任务停止下载后查询才是正确的。
	//调用后任务不会立即停止，需轮询到任务状态为PAUSE之后，才能获取到。
	std::function<BOOL(HANDLE hTask)> XL_StopTask;

	//强制暂停任务

	//	参数：[in] hTask，任务句柄
	//	返回值：TRUE表示成功，FALSE表示失败。当任务不存在时，调用会失败
	//	说明：在开启UAC下载大文件时，调用XL_StopTask可能需要消耗比较长的时间。
	//这种情况下如果需要快速暂停任务，可以调用XL_ForceStopTask，
	//不过有可能导致已经下载的还没写到磁盘的数据丢失。
	std::function<BOOL(HANDLE hTask)> XL_ForceStopTask;

	//查询任务当前信息。
	//参数：[in]hTask，任务句柄[out] stTaskInfo, 任务信息。
	//返回值：TRUE表示成功，FALSE表示失败。
	//说明：IsOriginUsable，原始资源是否可用，是一个布尔型的参数
	//	但是原始连接原始资源是一个耗时的操作，意味着中间有段时间原始资源是否有效是未知的。
	//这个参数的初始态是false，所以就存在原始资源是有效，但在任务刚开始时这个参数是false的问题。
	//上层在使用该值的时候需要根据业务逻辑再做处理。
	std::function<BOOL(HANDLE hTask, DownTaskInfo & stTaskInfo)> XL_QueryTaskInfoEx;

	//简单封装了XL_CreateTask接口
	std::function<HANDLE(const wchar_t *url, const wchar_t *path, const wchar_t *fileName, BOOL IsResume)> XL_CreateTaskByURL;

	//设置最大下载速度。
	//参数：[in]nKBps, 速度上限值
	//说明：异步执行
	std::function<void(INT32 nKBps)> XL_SetSpeedLimit;

	//限制上传速度
	//可根据实际情况设置外网和内网的上传速度。
	//	参数：[in] nTcpKBps, 内网上传速度，单位为KB / s[in] nOtherKBps, 外网上传速度，单位为KB / s
	//	返回值：无
	std::function<void(INT32 nTcpKBps, INT32 nOtherKBps)> XL_SetUploadSpeedLimit;

	//设置下载代理，全局任务代理信息。
	//	参数：[in]stProxyIfno 代理信息
	//返回值：TRUE表示成功，FALSE表示失败。参数非法返回FALSE
	std::function<BOOL(DOWN_PROXY_INFO &stProxyInfo)> XL_SetProxy;

	//设置向原始资源的Http任务请求的UserAgent。
	//	参数：[in]pszUserAgent，客户端UserAgent字符串
	//	说明：在调用该方法后创建的任务的才会使用新的UserAgent。
	std::function<void(const wchar_t *pszUserAgent)> XL_SetUserAgent;

	//简单封装了将迅雷专用链转成实际URL	
	//迅雷专用链是根据一定逻辑从普通URL转成的。
	//	参数：[in] pszThunderUrl，迅雷专用链URL[out] normalUrlBuffer，存储转化后的普通URL的缓冲区[out] bufferLen，normalUrlBuffer的缓冲区大小，单位字符数
	//	返回值：TRUE表示成功，FALSE表示失败
	//	说明：此函数功能独立，和其他函数没有依赖关系，可随时单独使用
	std::function<BOOL(const wchar_t *pszThunderUrl, wchar_t *normalUrlBuffer, INT32 bufferLen)> XL_ParseThunderPrivateUrl;


	//获取下载的文件大小
	std::function<BOOL(const wchar_t * lpURL, INT64& iFileSize)> XL_GetFileSizeWithUrl;

	std::function<BOOL(HANDLE hTask, char szFileId[40], unsigned __int64 nFileSize)> XL_SetFileIdAndSize;

	std::function<BOOL(HANDLE task_id, WSAPROTOCOL_INFOW *sock_info, CHAR *http_resp_buf, LONG buf_len)> XL_SetAdditionInfo;
	
	// 拉起迅雷7创建下载任务
	//通过传递URL和文件名等信息拉起迅雷7的新建面板创建下载任务。
	//	参数：[in] pszUrl，任务URL[in] pszFileName，下载保存的文件名[in] pszReferUrl，引用页URL[in] pszCharSet，当前网页的字符集[in] pszCookie，下载数据所需的cookie
	//	返回值：0成功，其他为失败
	std::function<LONG(wchar_t *pszUrl, wchar_t *pszFileName, wchar_t *pszReferUrl, wchar_t *pszCharSet, wchar_t *pszCookie)> XL_CreateTaskByThunder;

	std::function<LONG(const wchar_t *pszPath)> XL_CreateBTTaskByThunder;




private :

private:
	WisdomUtils::CDllLoader  *pDllloader;

	void				     *m_pCallbackParam;		//回调参数指针

	CallBackFunc		   m_callbackfuc;			//回调函数

	std::thread           *m_QueryTaskInfoThread;
	
	std::atomic<BOOL>      m_bRun;

	std::vector<HANDLE>    m_vecTask;
	
};

