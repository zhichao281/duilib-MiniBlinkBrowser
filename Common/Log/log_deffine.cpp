#define _CRT_SECURE_NO_WARNINGS
#include "log_deffine.h"
#include "Logger.h"
#include "LogManager.h"


extern std::unique_ptr<NLogger> g_plogger;

void  _log_write(EnumLogLevel nLevel, const char *pFmt, ...)
{
	if (g_plogger.get() == nullptr)
	{
		g_plogger.reset(new NLogger);
	}
	if (g_plogger->isRun() ==false)
	{
		return;
	}
	char pTemp[2048 * 100] = { 0 };
	va_list arg_ptr;
	va_start(arg_ptr, pFmt);
	vsprintf(pTemp + strlen(pTemp), pFmt, arg_ptr);
	va_end(arg_ptr);
	if (g_plogger&& g_plogger->isRun())
	{
		g_plogger->Write2Caching(nLevel, pTemp);
	}
}
void  _log_level(int nLevel)
{
	if (g_plogger.get() == nullptr)
	{
		g_plogger.reset(new NLogger);
	}
	g_plogger->SetLogLevel(nLevel);

}

void  _log_filename(const char *pfilename)
{
	if (g_plogger.get() == nullptr)
	{
		g_plogger.reset(new NLogger);
	}
	g_plogger->SetLogFileName(pfilename);
}