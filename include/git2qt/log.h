#ifndef LOG_H
#define LOG_H
#include <git2qt/enumtostringmap.h>

class QString;
namespace GIT {

class Log
{
public:
    enum LogLevel
    {
        Emergency		= 0,
        Alert           = 1,
        Critical        = 2,
        Error           = 3,
        Warning         = 4,
        Notice          = 5,
        Informational   = 6,
        Debug           = 7,
    };

    static void sysLogText(const char* file, int line, LogLevel level, const QString& text);

private:
    class LogLevelToStringMap : public EnumToStringMap<LogLevel>
    {
    public:
        LogLevelToStringMap()
        {
            insert(Emergency,       "EMG");
            insert(Alert,           "ALT");
            insert(Critical,        "CRT");
            insert(Error,           "ERR");
            insert(Warning,         "WRN");
            insert(Notice,          "NOT");
            insert(Informational,   "INF");
            insert(Debug,           "DBG");
        }
    };

    static const LogLevelToStringMap _LogLevelToStringMap;

};

#define KLOG_DEBUG      __FILE__,__LINE__,Log::LogLevel::Debug
#define KLOG_INFO       __FILE__,__LINE__,Log::LogLevel::Informational
#define KLOG_WARNING    __FILE__,__LINE__,Log::LogLevel::Warning
#define KLOG_ERROR      __FILE__,__LINE__,Log::LogLevel::Error
#define KLOG_FATAL      __FILE__,__LINE__,Log::LogLevel::Fatal

} // namspace GIT

#endif // LOG_H
