#include "processcontrol.h"
#include "queueprocessor.h"
#include <stdlib.h>
#include <base/net/socket.h>
#include <base/log/log.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <fstream>

void child_exit(int)
{
    exit(0);
}

MsgQueueProcessManager::MsgQueueProcessManager():_stop(false),_childPid(-1)
{
}


int MsgQueueProcessManager::Init(const std::string& configfile)
{
    _configFile = configfile;
    return 0;
}

bool MsgQueueProcessManager::ChildDead(int pid)
{
    return ((kill(pid, 0) != 0 && errno == ESRCH) || waitpid(pid, NULL, WNOHANG) == pid);
}

int MsgQueueProcessManager::Create(pid_t&pid, int& pipefd)
{
    int fd[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, fd) != 0)
    {
        LOG_ERROR("Open socket pair fail: %s", strerror(errno));
    }

    pid = fork();
    if (-1 == pid) 
    {
        printf("fork error:%d, errmsg:%s", errno, strerror(errno));
        ::close(fd[0]);
        ::close(fd[1]);
        return -1;
    }
    else if (0 == pid) 
    {
        printf("open child process:%d", pid);
        ::close(fd[0]);
        int ret = CreateProcess(fd[1]);
        exit(ret);
    }
    close(fd[1]);
    pipefd = fd[0];
    return 0;
}

int MsgQueueProcessManager::SendMsg(const char* buf, size_t len)
{
    while (true) 
    {
        int ret = ::send(_fd, buf, len, MSG_DONTWAIT);
        if (ret == -1 && errno == EINTR) 
        {
            continue;
        }

        if (ret <= 0) 
        {
            return -1;
        }
        buf += ret;
        len -= ret;

        if (len == 0) 
        {
            return 0;
        }
    }
}


void MsgQueueProcessManager::Run()
{
    do 
    {
        if(ChildDead(_childPid))
        {
            printf("child dead, start\n");
            int ret = Create(_childPid, _fd);
            printf("child dead, start ret:%d\n", ret);
        }
        usleep(500*1000);
    } while (!_stop);
}

void MsgQueueProcessManager::Stop()
{
    _stop = true;
    printf("stop process");
}

void MsgQueueProcessManager::Uninit()
{
    const char* msg = "stop";
    SendMsg(msg, strlen(msg));

    while( ::wait(NULL) != -1 || errno != ECHILD );

}

int MsgQueueProcessManager::CreateProcess(int fd)
{
    signal(SIGCHLD, SIG_DFL);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, SIG_IGN);
    signal(SIGTERM, child_exit);

    QueueProcessor app(fd);
    if( 0 != app.Open(_configFile))
    {
        printf("msg processor open error");
        return -1;
    }

    app.Run();

    app.Close();

    return 0;
}

