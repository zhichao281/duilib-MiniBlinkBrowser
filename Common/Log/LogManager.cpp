#include "LogManager.h"
CLogManager* CLogManager::m_gLogManager = NULL;


//class WSLogMessageHandler : public rtc::MessageHandler {
//public:
//	void OnMessage(rtc::Message* msg) override {
//		CLogManager::GetInstance()->DoWriteLog();
//	}
//};
//WSLogMessageHandler WsLogMessageHandler_;


CLogManager::CLogManager()
{
	bclose = false;

}
CLogManager::~CLogManager()
{
	bclose = true;
	if (thread)
	{
		thread->join();
		thread.reset();
	}

}

void CLogManager::AddLog(Logger* pLogger)
{
	m_loggerList.push_back(pLogger);
}

CLogManager* CLogManager::GetInstance()
{
	if (m_gLogManager == NULL)
	{
		m_gLogManager = new CLogManager();
		m_gLogManager->StartWriteLogThread();
	}
	return m_gLogManager;
}
void CLogManager::StartWriteLogThread()
{
	if (!thread)
	{
		thread = std::make_unique<std::thread>(std::bind(
			[=]()
		{
			CLogManager::GetInstance()->DoWriteLog();
		}		
		));
	}

}
void CLogManager::DoWriteLog()
{
	while(!bclose)
	{
 		Sleep(10);
		std::vector<Logger*> vcLogList;
		vcLogList = m_loggerList;
		for (int i = 0;i<vcLogList.size();i++)
		{
			if (vcLogList[i])
			{
				vcLogList[i]->DoWriteLog();
			}
		}
	}

}
