#ifndef __PROCESS_CONTROL_H__
#define __PROCESS_CONTROL_H__
#include <time.h>
#include <memory>
#include <string>
#include <vector>

class MsgQueueProcessManager
{
protected:
    int _childPid;
    int _fd;
    std::string _configFile;

public:
    MsgQueueProcessManager();
    int Init(const std::string& configFile);
    void Uninit();
    void Run();
    void Stop();

protected:
    bool  ChildDead(int pid);
    int SendMsg(const char* buf, size_t len);
    int CreateProcess(int fd);
    int Create(pid_t& pid, int& fd);

private:
    bool _stop;
};

#endif

