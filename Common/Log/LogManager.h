#pragma once
#include "Logger.h"
#include <windows.h>
#include <thread>
#include <atomic>
#include <mutex>

#define _CRT_SECURE_NO_WARNINGS

class CLoggerManager
{
private:
	CLoggerManager();

	~CLoggerManager();


public:
	static CLoggerManager* GetInstance();

    void AddLog(NLogger* pLogger);

	void DoWriteLog();

	void StartWriteLogThread();


private:
	static CLoggerManager* m_pLogManager;

	std::vector<NLogger*> m_loggerList;

	

	std::atomic< bool> m_bRunning; //线程是否在运行

};

