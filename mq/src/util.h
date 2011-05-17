#ifndef __UTIL_H_
#define __UTIL_H_
#include <unistd.h>
#include <vector>
#include <string>

#define  RUN_LOG(...) AC_LOG(base::LP_USER1, __VA_ARGS__);
#define  UDP_LOG(...) AC_LOG(base::LP_USER2, __VA_ARGS__);

namespace libconfig{
    struct Config;
    struct Setting;
}
namespace ac{
    class Logger;
}

enum LOGTYPE
{
    DEFAULT_LOGGER=0,
    UDP_LOGGER=1,
    RUN_LOGGER=2,
};

void ShowUsage(const char* program);

void ShowVersion();

int SwitchUser(const char* username);

void InitConfig(int argc, char *argv[], std::string & config);

void HandleExit(int signo);

int SetStackSize(size_t stackSize);

#endif

