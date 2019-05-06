#define _CRT_SECURE_NO_WARNINGS

#include "Logger.h"
#include <time.h>
#include <string>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <memory>
#include <crtdbg.h>

#include "LogManager.h"
#include <winsock2.h>   
#pragma comment(lib,"ws2_32.lib")  



// 
#ifdef _WIN32  
#include <direct.h>  
#include <io.h>  
#elif _LINUX  
#include <stdarg.h>  
#include <sys/stat.h>  
#endif 
#ifdef _WIN32  
#define ACCESS _access  
#define MKDIR(a) _mkdir((a))  
#elif _LINUX  
#define ACCESS access  
#define MKDIR(a) mkdir((a),0755)  
#endif  
int CreatDir(char *pDir)
{
	int i = 0;
	int iRet;
	int iLen;
	char pszDir[256];

	if (NULL == pDir)
	{
		return 0;
	}
	strcpy(pszDir, pDir);
	iLen = strlen(pszDir);

	// 创建中间目录  
	for (i = 0;i < iLen;i++)
	{
		if (pszDir[i] == '\\' || pszDir[i] == '/')
		{
			pszDir[i] = '\0';

			//如果不存在,创建  
			iRet = ACCESS(pszDir, 0);
			if (iRet != 0)
			{
				iRet = MKDIR(pszDir);
				if (iRet != 0)
				{
					return -1;
				}
			}
			//支持linux,将所有\换成/  
			pszDir[i] = '/';
		}
	}

	iRet = MKDIR(pszDir);
	return iRet;
}
std::string  GetExeName()
{
	//获取应用程序目录
	char szapipath[MAX_PATH];//（D:\Documents\Downloads\TEST.exe）
	memset(szapipath, 0, MAX_PATH);
	GetModuleFileNameA(NULL, szapipath, MAX_PATH);

	//获取应用程序名称
	char szExe[MAX_PATH] = "";//（TEST.exe）
	char *pbuf = NULL;
	char* szLine = strtok_s(szapipath, "\\", &pbuf);
	while (NULL != szLine)
	{
		strcpy_s(szExe, szLine);
		szLine = strtok_s(NULL, "\\", &pbuf);
	}

	//删除.exe
	strncpy_s(szapipath, szExe, strlen(szExe) - 4);
	return szapipath;

}

NLogger* NLogger::s_plogPtr = nullptr;
NLogger::GarbageCollector  NLogger::gc; //类的静态成员需要类外部初始化，

NLogger::NLogger(int log_level_,const char * strLogPath,const char* pStrFileName)
{
	m_pFileStream = NULL;
	char scurdir[1024];
	char sBuf[1024];
	_getcwd(scurdir,1024);   // 
	sprintf(sBuf,"%s/%s",scurdir,strLogPath);
	printf("LogPath:%s\n",sBuf);
	m_strLogPath = sBuf;
	m_strCurLogName = GetExeName().c_str();
	m_nCurrentDay = 0;
	m_log_level_ = log_level_;
	CreateLogPath();
	GenerateLogName();
	m_bRunning.store(true);
	CLoggerManager::GetInstance()->AddLog(this);
}
int gettimeofday(struct timeval *tp, void *tzp)
{
	time_t clock;
	struct tm _tm;
	SYSTEMTIME wtm;
	GetLocalTime(&wtm);
	_tm.tm_year = wtm.wYear - 1900;
	_tm.tm_mon = wtm.wMonth - 1;
	_tm.tm_mday = wtm.wDay;
	_tm.tm_hour = wtm.wHour;
	_tm.tm_min = wtm.wMinute;
	_tm.tm_sec = wtm.wSecond;
	_tm.tm_isdst = -1;
	clock = mktime(&_tm);
	tp->tv_sec = clock;
	tp->tv_usec = wtm.wMilliseconds * 1000;
	return (0);
}

std::string GetWsCurrentTime()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	struct tm * pTimeInfo = NULL;
	time_t time_t_ = tv.tv_sec;
	pTimeInfo = localtime(&time_t_);
	char temp[2048] = { 0 };
	sprintf(temp, "%04d-%02d-%02d %02d:%02d:%02d.%03d ", pTimeInfo->tm_year + 1900, pTimeInfo->tm_mon + 1, pTimeInfo->tm_mday, pTimeInfo->tm_hour, pTimeInfo->tm_min, pTimeInfo->tm_sec, tv.tv_usec / 1000);
	std::string pTemp = temp;
	return pTemp;
}
NLogger::~NLogger()
{
	m_bRunning.store(false);
 	if (m_pFileStream)
	{
		fclose(m_pFileStream);
		m_pFileStream = NULL;
	}
	m_vcStrList.clear();


	
}
bool  NLogger::isRun()
{
	return m_bRunning.load();
}

void  NLogger::Stop()
{
	m_bRunning.store(false);
	return;
}
void NLogger::Write2Caching(int log_level_,const char * strInfo, ...)
{

	if (!m_bRunning.load())
	{
		return;
	}

	if (log_level_  < m_log_level_)
	{
		printf("return log_level_ < m_log_level_");
		return;
	}

	if(!strInfo)
	{
		printf("return strInfo");
		return;
	}
	char pTemp[2048*100] = {0};
	strcpy(pTemp, GetWsCurrentTime().c_str());
	strcat(pTemp, logLevelToString(log_level_));
	va_list arg_ptr;
	va_start(arg_ptr, strInfo);
	vsprintf(pTemp + strlen(pTemp), strInfo, arg_ptr);
	va_end(arg_ptr);
	OutputDebugStringA(pTemp);
	{
		std::unique_lock<std::mutex> _lock(mutex_);
		m_vcStrList.push_back(pTemp);
	}


}
void NLogger::Write2Caching(const char * strInfo)
{
	if (!m_bRunning.load())
	{
		return;
	}
	std::unique_lock<std::mutex> _lock(mutex_);
	m_vcStrList.push_back(strInfo);

}

const char* NLogger::logLevelToString(int l) {
	switch ( l ) {
			case LogLevelTrace:
				return "TRACE";
			case LogLevelDebug:
				return "DEBUG";
			case LogLevelInfo:
				return "INFO";
			case LogLevelWarring:
				return "WARN" ;
			case LoglevelError:
				return "ERROR";
			default:
				return "UNKNOWN";
	}
}



std::string NLogger::Time2String(time_t time_t_)
{
	if (time_t_ == 0)
	{
		return "";
	}
	struct tm * pTimeInfo = NULL;
	pTimeInfo = localtime(&time_t_);
	char temp[2048] = {0};
	sprintf(temp, "%04d-%02d-%02d",pTimeInfo->tm_year+1900,pTimeInfo->tm_mon+1,pTimeInfo->tm_mday);
	std::string pTemp = temp;
	return pTemp;	
}

// DoWriteLog
void NLogger::DoWriteLog()
{
	if (!m_pFileStream || !m_bRunning.load())
	{
		printf("m_pFileStream NULL");
		return;
	}
	std::vector<std::string> vcStrlist;
	mutex_.lock();
	vcStrlist = m_vcStrList;
	m_vcStrList.clear();
	mutex_.unlock();
	for (int i = 0;i<vcStrlist.size();i++)
	{
		if (!m_pFileStream || !m_bRunning.load())
		{
			printf("m_pFileStream NULL");
			return;
		}
		fprintf(m_pFileStream, "%s", vcStrlist[i].c_str());
	    printf("%s",vcStrlist[i].c_str());
	}
	fflush(m_pFileStream);
	GenerateLogName();	
}

void  NLogger::SetLogLevel(int log_level_)
{
	m_log_level_ = log_level_;

}
void  NLogger::SetLogFileName(const char * pStrFileName)
{
	if (m_strCurLogName != pStrFileName)
	{
		m_strCurLogName = pStrFileName;

		time_t curTime = time(NULL);
		curTime = curTime / (3600 * 24);
		char temp[1024] = { 0 };
		m_nCurrentDay = curTime;
		//2013-01-01.log
		sprintf(temp, "%s/%s-%s.log", m_strLogPath.c_str(), Time2String(m_nCurrentDay*(3600 * 24)).c_str(), m_strCurLogName.c_str());
		if (m_pFileStream)
		{
			fclose(m_pFileStream);
			m_pFileStream = NULL;
		}
		m_pFileStream = fopen(temp, "ab+");
		for (int i = 0; i < 100; i++)
		{
			if (!m_pFileStream)
			{
				sprintf(temp, "%s/%s-%s_%03d.log", m_strLogPath.c_str(), Time2String(m_nCurrentDay*(3600 * 24)).c_str(), m_strCurLogName.c_str(), i);
				m_pFileStream = fopen(temp, "ab+");
				if (m_pFileStream)
				{
					break;
				}
			}
		}
	}
;
}
NLogger * NLogger::GetInstance()
{
	if (NULL != s_plogPtr)
	{
		return s_plogPtr;
	}

	s_plogPtr = new NLogger();
	
	//atexit(Destory);
	return s_plogPtr;
}
//������־�ļ������
void NLogger::GenerateLogName()
{
	time_t curTime = time(NULL);
	curTime = curTime/(3600*24);
	if (curTime > m_nCurrentDay)
	{
		char temp[1024] = {0};
		m_nCurrentDay = curTime;
		//2013-01-01.log
		sprintf(temp, "%s/%s-%s.log",m_strLogPath.c_str(),Time2String(m_nCurrentDay*(3600*24)).c_str(),m_strCurLogName.c_str());
		if(m_pFileStream)
		{
			fclose(m_pFileStream);
			m_pFileStream = NULL;	
		}
		m_pFileStream = fopen(temp, "ab+");
		for (int i = 0;i<100;i++)
		{
			if (!m_pFileStream)
			{
				sprintf(temp, "%s/%s-%s_%03d.log", m_strLogPath.c_str(), Time2String(m_nCurrentDay*(3600 * 24)).c_str(), m_strCurLogName.c_str(),i);
				m_pFileStream = fopen(temp, "ab+");
				if (m_pFileStream)
				{
					break;
				}
			}
		}
		
	}
	
}
void NLogger::CreateLogPath()
{
	CreatDir((char*)m_strLogPath.c_str());
}
