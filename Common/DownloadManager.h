#pragma once
#include <stdlib.h>
#include <mutex>

#include "../../Common/libcurl.h"
#include "../../Common/HSingleton.h"
#include "../../Common/NCom.h"


using namespace std;
class CDownloadManager 
{
public:
	CDownloadManager();

	~CDownloadManager();
public:
	//新增下载任务
	bool  AddDownTask(std::string strGameID, std::string strUrl, std::string strloadPath, CallBackFunc callbackfuc = nullptr, void* lpParam = nullptr);
	
	//执行下载操作
	 void DownloadTask(std::string strGameID, std::string strUrl, std::string strloadPath, CallBackFunc callbackfuc = nullptr, void* lpParam = nullptr);
	
	//暂停下载任务
	void  PauseDownTask(std::string strGameID);

	//继续下载任务
	bool  ResumeDownTask(std::string strGameID);

	//删除下载任务
	void  DelDownTask(GameInfo &gInfo);

	//删除下载任务
	void  DelDownTask();

	//读取上次运行时间
	clock_t  ReadTime(std::string strGameID);

	//重置上次运行时间
	void  ResetTime(std::string strGameID);

	//删除下载任务
	 void  SafeDel(std::string strGameID, CDownloader *downloader);

	 //获取当前下载任务个数
	int  GetTaskSize();
private:
	std::map<string, CDownloader*>  m_mapGidToDownload;

	std::map<string, clock_t>  m_mapGidToTime;
	std::mutex m_mutex;
	
};

#define gblDownloadMgrGet HSingletonTemplatePtr<CDownloadManager>::Instance()
