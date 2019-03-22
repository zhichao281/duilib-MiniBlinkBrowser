// +----------------------------------------------------------------------
// | C++ Basis Components
// +----------------------------------------------------------------------
// | GSLog		日志
// +----------------------------------------------------------------------
// | Author:	
// +----------------------------------------------------------------------
// | Modify:	
// +----------------------------------------------------------------------

#pragma  once

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <stdarg.h>


// 日志等级
typedef enum _GSLOG_LEVEL_
{
	LEVEL_ERROR		= 0,
	LEVEL_WARN		= 1,
	LEVEL_DEBUG		= 2,
	LEVEL_INFO		= 3,
	LEVEL_ALL		= 4,
	LEVEL_CLOSE		= 5
}GSLOG_LEVEL;

class GSLog
{

public:

	// 构造函数
	GSLog();

	// 析构函数
	virtual ~GSLog();

protected:

	// 初始化
	void _Init();

	// 解析路径
	void _ParsePath(char *pPath, char* pDir, char *pName);

	// 获取当前日期
	void _GetCurTime(char *pDate = NULL, char *pTime = NULL);

	// 检测日志文件时间
	bool _CheckFileTime();

	// 重新创建文件
	void _Rotate();

	// 创建文件夹
	int CreateDir(const char* pDir);
	// 配置文件
	void Config(const char* pConfigFile);
public:

	// 打开日志文件
	bool Open(char *pFilePath, GSLOG_LEVEL nLevel = LEVEL_DEBUG);

	// 写日志
	int Logv(GSLOG_LEVEL nLevel, const char *pFmt, va_list vaList);

	// 写错误日志
	int LogError(const char *pFmt, ...);

	// 写警告日志
	int LogWarn(const char *pFmt, ...);

	// 写调试日志
	int LogDebug(const char *pFmt, ...);

	// 写信息日志
	int LogInfo(const char *pFmt, ...);

	// 关闭
	void Close();


private:

	GSLOG_LEVEL			m_nLevel;
	FILE*				m_pFile;
	CRITICAL_SECTION	m_Mutex;
	char				m_szLogPath[1024];
	char				m_szName[1024];
	char				m_szDir[1024];
	char				m_szCurDate[128];
};

void _log_init(const char *pFilePath, GSLOG_LEVEL nLevel = LEVEL_DEBUG);
int _log_write(GSLOG_LEVEL nLevel, const char *pFmt, ...);

#define GSLOG_INIT(pFilePath, nLevel) \
	_log_init(pFilePath, nLevel)


#define GSLOG_INFO(fmt, ...) _log_write(LEVEL_INFO, "%s(%d): " fmt, __FILE__, __LINE__, __VA_ARGS__)

#define GSLOG_ERROR(fmt, ...) _log_write(LEVEL_ERROR, "%s(%d): " fmt, __FILE__, __LINE__, __VA_ARGS__)

#define GSLOG_WARN(fmt, ...) _log_write(LEVEL_WARN, "%s(%d): " fmt, __FILE__, __LINE__, __VA_ARGS__)

#define GSLOG_DEBUG(fmt, ...) _log_write(LEVEL_DEBUG, "%s(%d): " fmt, __FILE__, __LINE__, __VA_ARGS__)





