#include "asyncfilelogger.h"
#include <pthread.h>

namespace base{

#define  AYNCFILELOG_BUFSIZE    1024*1024*4
#define  AYNCFILELOG_BLOCKSIZE  1024*4

int logData(int fd, const std::string& logFile, char* data, size_t /*size*/)
{
    static std::string currentLogFileName; 
    if (logFile != currentLogFileName)
    {
        if (fd > 0)
        {
            close(fd);
        }
        fd =-1;
    }

    if(fd < 0)
    {
        FILE* fp = fopen(logFile.c_str(), "a+");
        if (fp == NULL)
        {
            return -1;
        }
        fd = fileno(fp);
        time_t cur_time = time(NULL);
        printf("Open new log file %s at %s, old file:%s", tmpLogFileName.c_str(), ctime(&cur_time), currentLogFileName.c_str());
        currentLogFileName = logFile;
    }
    if ( fd < 0) 
    {
        printf("open file %s error", tmpLogFileName.c_str());
        return -1;
    }
    fprintf(fp, "%s\n", data);
    return fd;
}

void* __THREAD_FUNC(void* p)
{
    
    AsyncFileLogger* logger = static_cast<AsyncFileLogger*>(p);
    assert(logger);
    int fd = -1;
    std::string currentLogFileName; 
    
    char* buffer = new char[AYNCFILELOG_BUFSIZE];
    char* curPos = 0;
    char* endPos = 0;
    while(logger->IsLog())
    {
        if (curPos < endPos)
        {
            fd = logData(fd, logger->GetLogFileName(), curPos, endPos-curPos);
            curPos = endPos;
        }
        usleep(500);
    }
}

AsyncFileLogger::AsyncFileLogger(const std::string & sLogFile/* = */, const std::string & sLogFormat/* =Logger::DEFAULT_FORMAT */, int iMode/* =0644 */):
DateFileLogger(sLogFile, sLogFormat, iMode)
{
    _log = true;
    pthread_attr_t attr;
    if ( pthread_attr_init(&attr) != 0 ) {
        return -1;
    }

    if ( pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE) != 0 ) {
        return -1;
    }

    if (pthread_create(&_htd, &attr, __THREAD_FUNC, this) == -1) {
        return -1;
    }
}

AsyncFileLogger::~AsyncFileLogger()
{
    _log = false;
}

int AsyncFileLogger::Log(LogPriority iLogPriority, const std::string & sLogMsg)
{
    
}

}