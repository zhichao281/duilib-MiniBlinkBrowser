#include "LogManager.h"
CLoggerManager* CLoggerManager::m_pLogManager = NULL;


//class WSLogMessageHandler : public rtc::MessageHandler {
//public:
//	void OnMessage(rtc::Message* msg) override {
//		CLogManager::GetInstance()->DoWriteLog();
//	}
//};
//WSLogMessageHandler WsLogMessageHandler_;


CLoggerManager::CLoggerManager()
{
	m_bRunning.store(false);

}
CLoggerManager::~CLoggerManager()
{

	m_bRunning.store(false);



}

void CLoggerManager::AddLog(NLogger* pLogger)
{
	m_loggerList.push_back(pLogger);
}

CLoggerManager* CLoggerManager::GetInstance()
{
	if (m_pLogManager == NULL)
	{
		m_pLogManager = new CLoggerManager();
		//m_pLogManager->StartWriteLogThread();
	}
	return m_pLogManager;
}
void CLoggerManager::StartWriteLogThread()
{
	if (m_bRunning.load()==true)
	{
		return;
	}
	
	std::unique_ptr<std::thread> m_thread;
	m_thread = std::make_unique<std::thread>(std::bind(
		[=]()
	{
		m_bRunning.store(true);
		DoWriteLog();
	}
	));
	m_thread->detach();


}
void CLoggerManager::DoWriteLog()
{
	while(m_bRunning.load() == true )
	{
 		Sleep(10);
		std::vector<NLogger*> vcLogList;
		vcLogList = m_loggerList;
		for (size_t i = 0;i<vcLogList.size();i++)
		{
			if (vcLogList[i] && vcLogList[i]->isRun() && m_bRunning.load())
			{
				vcLogList[i]->DoWriteLog();
			}	
			if (vcLogList[i]->isRun() ==false)
			{
				m_bRunning.store(false);
				return;
			}
		}
	}
	
	
}
