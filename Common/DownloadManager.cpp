
#include "DownloadManager.h"



#include "../../Common/StringUtilities.h"
#include "../../Common/thread_pool/thread_pool.h"
#include "../Config/Config.h"
#include "Utils.h"



using namespace netlib;
CDownloadManager::CDownloadManager()
{

}

CDownloadManager::~CDownloadManager()
{

}


bool  CDownloadManager::AddDownTask(std::string strGameID, std::string strUrl, std::string strloadPath, CallBackFunc callbackfuc, void* lpParam)
{
	CDownloader *pDownloader = nullptr;
	{
		std::unique_lock<std::mutex> guard(m_mutex);
		pDownloader = m_mapGidToDownload[strGameID];
		if (pDownloader != nullptr )
		{
			if (pDownloader->isRunning() == true)
			{
				return false;
			}
			else
			{
				SafeDel(strGameID.c_str(), pDownloader);
			}
		
		}	

	}

	GSThreadPool_AddFun(std::bind(&CDownloadManager::DownloadTask, this, strGameID, strUrl, strloadPath, callbackfuc, lpParam));
	m_mapGidToTime[strGameID] = clock();

	return true;
}

void  CDownloadManager::DownloadTask(std::string strGameID, std::string strUrl, std::string strloadPath, CallBackFunc callbackfuc, void* lpParam)
{
	clock_t start = clock();
	bool bRun=true;
	int nError = 0;
	
	GameInfo *ptr = (GameInfo *)lpParam;
	GameInfo gameinfo = *ptr;
	delete ptr;
	ptr = nullptr;

	while (bRun)
	{
		CDownloader *pDownloader = nullptr;
		{
			std::unique_lock<std::mutex> guard(m_mutex);
			pDownloader = m_mapGidToDownload[strGameID];
			if (pDownloader == nullptr)
			{
				pDownloader = new CDownloader();
				m_mapGidToDownload[strGameID] = pDownloader;
				GameInfo *pGameInfo = new GameInfo();
				*pGameInfo = gameinfo;
				pDownloader->setCallback(callbackfuc, (void *)pGameInfo);
			}
			else
			{
				bRun = false;
				return;
			}
		}

		bool bres = pDownloader->start(strUrl.c_str(), strloadPath.c_str());
		pDownloader->setSpeed(Config::GetDownSpeed());
		if (bres == false)
		{
			SafeDel(strGameID.c_str(), pDownloader);
			return;
		}
		bool ret = pDownloader->run();
		if (ret == false && pDownloader->isRunning() == true)
		{		

			if (pDownloader->getState() != DOWNLOADSTATES::DOWN_CONTINUE)
			{
				nError++;
				if (nError > 4)
				{
					pDownloader->stop();
					SafeDel(strGameID.c_str(), pDownloader);
					return;
				}
			}
			SafeDel(strGameID.c_str(), pDownloader);
			Sleep(1000);
			continue;
		}
		else
		{
			bRun = false;
			SafeDel(strGameID.c_str(), pDownloader);
			return;
		}
	}
	return ;

}

void  CDownloadManager::SafeDel(std::string strGameID, CDownloader *pDownloader)
{
	if (pDownloader != nullptr)
	{
		delete pDownloader;
		pDownloader = nullptr;
		{
			//std::unique_lock<std::mutex> guard(m_mutex);		
			m_mapGidToDownload[strGameID] = nullptr;
			m_mapGidToDownload.erase(strGameID.c_str());		
		}	
	}
}

int  CDownloadManager::GetTaskSize()
{
	return  m_mapGidToDownload.size();
}

//暂停下载任务
void  CDownloadManager::PauseDownTask(std::string strGameID)
{
	m_mutex.lock();
	CDownloader *downloader = m_mapGidToDownload[strGameID];
	m_mutex.unlock();
	if (downloader != nullptr)
	{
		downloader->pause();
	}

}

//继续下载任务
bool  CDownloadManager::ResumeDownTask(std::string strGameID)
{
	m_mutex.lock();
	CDownloader *downloader = m_mapGidToDownload[strGameID];
	m_mutex.unlock();
	if (downloader != nullptr)
	{
		downloader->resume();
	}
	else
	{

		return false;
	}

	return true;
}

//删除下载任务
void  CDownloadManager::DelDownTask(GameInfo &gInfo)
{
	m_mutex.lock();
	std::map<string, CDownloader*>::iterator it = m_mapGidToDownload.find(gInfo.strGameID);
	m_mutex.unlock();

	if (it != m_mapGidToDownload.end() && it->second != nullptr)
	{
		it->second->SetDeleteFlag(true);
		it->second->stop();
	}
	else
	{
		wstring wstrTmpPath = StringUtilities::str2wstr(gInfo.strDownPath + "\\" + Utils::GetInfoFromPackUrl(gInfo.strSourceUrl) + ".tmp");
		if (!DeleteFile(wstrTmpPath.c_str()))
		{
			MoveFileEx(wstrTmpPath.c_str(), nullptr, MOVEFILE_DELAY_UNTIL_REBOOT);
		}
	}
}

//删除下载任务
void  CDownloadManager::DelDownTask()
{	
	if (m_mapGidToDownload.size()<1)
	{
		return;
	}
	m_mutex.lock();
	for (auto it = m_mapGidToDownload.begin(); it != m_mapGidToDownload.end(); ++it)
	{
		CDownloader *downloader = it->second;
		if (downloader != nullptr)
		{
			downloader->stop();
		}
	}
	m_mutex.unlock();

	//Sleep(100);
	//netlib::ThreadPool::Destory();

}

clock_t  CDownloadManager::ReadTime(std::string strGameID)
{
	clock_t  start = m_mapGidToTime[strGameID];

	return start;

}

void  CDownloadManager::ResetTime(std::string strGameID)
{
	m_mapGidToTime[strGameID] = clock();
	return;
}