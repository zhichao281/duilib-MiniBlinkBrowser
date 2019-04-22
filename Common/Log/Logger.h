#ifndef LOGGER_H_
#define LOGGER_H_
#include <stdio.h>
#include "log_deffine.h"
#include <vector>
#include <mutex>


class Logger
{
public:
	// 
	Logger(int log_level_ = LogLevelDebug,const char * strLogPath = "Log",const char* pStrFileName = "client");
	// 
	virtual ~Logger();
public:	
	// 
	void  Write2Caching(int log_level_,const char * strInfo, ...);

	void  Write2Caching(const char * strInfo);
	

	void DoWriteLog();

	void  SetLogLevel(int log_level_);

	void  SetLogFileName(const char * pStrFileName);



	// 获取单例实例
	static Logger* GetInstance();

	static void Destory()
	{
		if (nullptr != s_plogPtr)
		{
			delete s_plogPtr;
			s_plogPtr = nullptr;
		}
	};

private:

	std::string Time2String(time_t time_t_);
	// 
	void GenerateLogName();

	// 
	void CreateLogPath();

	const char* logLevelToString(int l);




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
	static Logger*		s_plogPtr;

};
#endif

