#pragma once
#include "Logger.h"


#define _WINSOCKAPI_

#include <windows.h>

#include <thread>
#define _CRT_SECURE_NO_WARNINGS
class CLogManager
{
private:
	CLogManager();
	~CLogManager();
	static CLogManager* m_gLogManager;
	
	std::vector<Logger*> m_loggerList;
	std::unique_ptr<std::thread> thread;
	bool bclose;
public:
	static CLogManager* GetInstance();
    void AddLog(Logger* pLogger);
	void DoWriteLog();
	void StartWriteLogThread();


};

