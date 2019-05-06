#include "LogManager.h"
CLoggerManager* CLoggerManager::s_pLogManager = NULL;

CLoggerManager::GarbageCollector  CLoggerManager::gc; //类的静态成员需要类外部初始化，

CLoggerManager::CLoggerManager()
{
	m_bRunning.store(false);

}
CLoggerManager::~CLoggerManager()
{

	m_bRunning.store(false);

	if (m_thread)
	{
		m_thread->join();
		m_thread.reset();
	}
	m_loggerList.clear();
}


void CLoggerManager::AddLog(NLogger* pLogger)
{
	m_loggerList.push_back(pLogger);
}

CLoggerManager* CLoggerManager::GetInstance()
{
	if (s_pLogManager == NULL)
	{
		s_pLogManager = new CLoggerManager();
		s_pLogManager->StartWriteLogThread();
		//atexit(Destory);
	}
	return s_pLogManager;
}
void CLoggerManager::StartWriteLogThread()
{
	if (m_bRunning.load() == true)
	{
		return;
	}
	if (!m_thread)
	{
		m_thread = std::make_unique<std::thread>(std::bind([=]()
		{
			m_bRunning.store(true);
			DoWriteLog();
		}		
		));
	}

}

void CLoggerManager::Stop()
{
	m_bRunning.store(false);
	
}

void CLoggerManager::DoWriteLog()
{
	while(m_bRunning.load())
	{ 		
		std::vector<NLogger*> vcLogList;
		vcLogList = m_loggerList;
		for (int i = 0;i<vcLogList.size();i++)
		{
			if (!m_bRunning.load())
			{
				return;
			}
			if (vcLogList[i] && m_bRunning.load())
			{
				vcLogList[i]->DoWriteLog();
			}
		}
		if (!m_bRunning.load())
		{
			return;
		}
		Sleep(10);
	}

}
