#include "util.h"
#include <pwd.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <string>
#include <memory>
#include <sys/time.h>
#include <sys/resource.h>
#include <base/log/log.h>
#include <base/config/libconfig.h++>


const char* option_format = "f:vh";

static option long_options[] = {
    {"config-file", 1, 0, 'f'},    
    {"version", 0, 0, 'v'},
    {"help", 0, 0, 'h'},
    {NULL, 0, 0, 0}
};

void ShowUsage(const char* program) 
{
    printf("Usage: %s [OPTION]...\n\n", program);
    printf(
        "Mandatory arguments to long options are mandatory for short options too.\n"
        "  -f, --config-file=PATH         init program from a config file\n"
        "  -v, --version                  display version\n"
        "  -h, --help                     display this help and exit\n"
        "\n"
        );
}


const char* __programversion;

void ShowVersion()
{
    printf("%s \n", __programversion/*STR(PROGRAM_VERSION)*/);
}


void InitConfig(int argc, char *argv[], std::string& configFile)
{
    while(true)
    {
        int option_index = 0;

        int c = getopt_long(argc, argv, option_format, long_options, &option_index);
        if (c == -1) break;

        switch(c)
        {
        case 0: // long option, not need process
            break;
        case 'f':
            configFile.assign(optarg);
            break;
        case 'v':
            ShowVersion(); exit(0);
        case 'h':
            ShowUsage(argv[0]); exit(0);
        }
    }
}

/*base::Logger * CreateLogger(libconfig::Setting & log)
{
    const char * locallog = (const char*)log;

    if ( strlen(locallog) == 0 ) {
        return NULL;
    }

    if ( strncmp(locallog, "udp:", 4) == 0 ) {

        char szBuffer[256] = {0};
        strncpy(szBuffer, locallog+4, sizeof(szBuffer)-1);

        char * pIPBegin = szBuffer;
        char * pIPEnd = strstr(pIPBegin, ":");
        if ( !pIPEnd ) {
            return NULL;
        }
        *pIPEnd = 0;

        char * pPortBegin = pIPEnd + 1;
        if ( !pPortBegin ) {
            return NULL;
        }

        std::string sFormat = base::Logger::DEFAULT_FORMAT;
        char * pFmtBegin = strstr(pPortBegin, ":");
        if ( pFmtBegin ) {
            sFormat.assign(pFmtBegin + 1);
        }

        return new base::UdpLogger(base::INetAddr(pIPBegin, atoi(pPortBegin)), sFormat);
    }
    else {
        char filename[256] = {0};
        strncpy(filename, locallog, sizeof(filename)-1);
        std::string format = base::Logger::DEFAULT_FORMAT;
        char* fmt = strstr(filename, ":");
        if (fmt) {
            *fmt++ = '\0';
            format = fmt;
        }

        base::DateFileLogger * pLogger = new base::DateFileLogger(filename, format);
        if ( pLogger ) {
            pLogger->SetLogFile(filename);
        }
        return pLogger;
    }
}*/

void SetLogger(const libconfig::Setting& setting)
{
    /*base::Logger * pLogger = CreateLogger(setting["Log"]);
    base::Logger * udpLogger = CreateLogger(setting.exists("UdpLog") ? setting["UdpLog"] : setting["Log"]); 
    base::Logger * runloger = CreateLogger(setting.exists("RunLog") ? setting["RunLog"] : setting["Log"]);
    for (std::vector<base::Logger*>::iterator it = __loggers__.begin(); it != __loggers__.end(); ++it)
    {
        if (*it && *it != base::NullLogger::Instance())
        {
            delete *it;
        }
    }
    __loggers__.clear();
    __loggers__.push_back(pLogger);
    __loggers__.push_back(udpLogger);
    __loggers__.push_back(runloger);
    if (pLogger!= NULL)
    {
        AC_SET_DEFAULT_LOGGER(pLogger);
    }

    if (runloger != NULL)
    {
        AC_SET_LOGGER(base::LP_USER1, runloger);
    }
    else
    {
        AC_SET_LOGGER(base::LP_USER1, base::NullLogger::Instance());
    }

    if ( udpLogger != NULL ) 
    {
        LOG_INFO("udp log:%s", (const char*)setting["UdpLog"]);
        AC_SET_LOGGER(base::LP_USER2, udpLogger);
    }
    else 
    {
        AC_SET_LOGGER(base::LP_USER2, base::NullLogger::Instance());
    }
    */
}

/*base::Logger* GetLogger(LOGTYPE type)
{
    if (__loggers__.size() < (size_t)type)
    {
        return NULL;
    }
    return __loggers__[type];
}
*/
void HandleExit(int signo)
{
    (void)signo;
    LOG_INFO("recv signal[%d] stop", signo);
   
}

int SetStackSize(size_t stackSize)
{
    struct rlimit limit;
    if (getrlimit(RLIMIT_STACK, &limit) != 0) 
    { 
        return -1;
    }
    if (limit.rlim_cur < stackSize) 
    { 
        limit.rlim_cur = stackSize; 
        if (setrlimit(RLIMIT_STACK, &limit) != 0) 
        { 
            return -1;
        } 
    } 
    return 0;
}

