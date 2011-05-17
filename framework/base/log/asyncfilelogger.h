#ifndef _AC_LOG_ASYNC_FILELOGER_H_
#define _AC_LOG_ASYNC_FILELOGER_H_
namespace base {

class AsyncFileLogger: public base::DateFileLogger
{
public:
    AsyncFileLogger(const std::string & sLogFile="", const std::string & sLogFormat=Logger::DEFAULT_FORMAT,  int iMode=0644);
    virtual int Log(LogPriority iLogPriority, const std::string & sLogMsg);

    bool IsLog() { return _log;}
    ~AsyncFileLogger();

private:
    int _htd;
    bool _log;
};

}



#endif