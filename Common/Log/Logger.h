#ifndef LOGGER_H_
#define LOGGER_H_


#include <stdio.h>
#include <vector>
#include <mutex>
#include <atomic>

#include "log_deffine.h"


class NLogger
{
public:
	// 
	NLogger(int log_level_ = LogLevelDebug,const char * strLogPath = "Log",const char* pStrFileName = "client");
	// 
	virtual ~NLogger();
public:	
	// 
	void  Write2Caching(int log_level_,const char * strInfo, ...);

	void  Write2Caching(const char * strInfo);
	

	void DoWriteLog();

	void  SetLogLevel(int log_level_);

	void  SetLogFileName(const char * pStrFileName);

	bool  isRun();

	void  Stop();


	// 获取单例实例
	static NLogger* GetInstance();

	//static void Destory()
	//{
 //		if (nullptr != s_plogPtr)
	//	{
	//		delete s_plogPtr;
	//		s_plogPtr = nullptr;
	//	}
	//};

private:

	std::string Time2String(time_t time_t_);
	// 
	void GenerateLogName();

	// 
	void CreateLogPath();

	const char* logLevelToString(int l);


	// This is important
	class GarbageCollector  // 垃圾回收类
	{
	public:

		~GarbageCollector()
		{
			// We can destory all the resouce here, eg:db connector, file handle and so on
			if (s_plogPtr != nullptr)
			{
				delete s_plogPtr;
				s_plogPtr = NULL;		
			}
		}
	};
	static GarbageCollector  gc;  //垃圾回收类的静态成员


private:
	// 
	FILE * m_pFileStream;

	std::string m_strLogPath;

	int m_log_level_;

	std::string m_strCurLogName;

	

	std::vector<std::string> m_vcStrList;

	int  m_nCurrentDay;
	std::mutex  mutex_;
public:
	static NLogger*		s_plogPtr;

	std::atomic< bool> m_bRunning; //线程是否在运行

};
#endif

