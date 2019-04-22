#ifndef  _LOG_DEFINE_
#define  _LOG_DEFINE_
//============basic===================
#include <memory>
typedef enum EnumLogLevel
{
	LogLevelTrace =  0,
	LogLevelDebug =  1,
	LogLevelInfo  =  2,  
	LogLevelWarring  =  3,
	LoglevelError =  4,
	loglevelClose =5
};


void  _log_write(EnumLogLevel nLevel, const char *pformat, ...);

void  _log_level(int nLevel);

void  _log_filename(const char *pfilename);

#define  LOG_LEVEL( _level ) _log_level(_level);

#define  LOG_FILENAME( _file ) _log_filename(_file);

#define LOG_INFO(format, ...) _log_write(LogLevelInfo, ": [%s<%04d>] "format"\r\n", __FUNCTION__, __LINE__,  ##__VA_ARGS__)

#define LOG_ERROR(format, ...) _log_write(LoglevelError, ": [%s<%04d>] "format"\r\n", __FUNCTION__, __LINE__, ## __VA_ARGS__)

#define LOG_WARN(format, ...) _log_write(LogLevelWarring, ": [%s<%04d>] "format"\r\n", __FUNCTION__, __LINE__, ## __VA_ARGS__)

#define LOG_DEBUG(format, ...) _log_write(LogLevelDebug, ": [%s<%04d>] "format"\r\n", __FUNCTION__, __LINE__, ## __VA_ARGS__)


#define  WRITE_FILE(filename,buf,size) { FILE* file = fopen(filename,"ab+"); fwrite(buf,1,size,file);fclose(file);}
#endif
