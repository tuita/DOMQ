#include <string>
#include <iostream>
#include <stdio.h>
#include <base/log/log.h>
#include <base/config/libconfig.h++>
#include <netmgr/connectionmgr.h>
#include <netmgr/message.h>
#include <base/third/scope_guard.h>
#include <base/util/util.h>
#include <netmgr/msgfactory.h>
#include <signal.h>
#include "util.h"
#include "processcontrol.h"
#include "queuemsgfactory.h"
#include "queueprocessor.h"

MsgQueueProcessManager* processMgr = NULL;

void prog_exit(int)
{
    if (processMgr) 
    {
        processMgr->Stop();
    }
}

int main(int ac, char ** av)
{
    if ( ac == 1 ) 
    {
        ShowUsage(av[0]);
        return 0;
    }

    std::string configFile;
    InitConfig(ac, av, configFile);

    if (configFile == "")
    {
        ShowUsage(av[0]);
        return 0;
    }

    signal(SIGINT, prog_exit);
    signal(SIGCHLD, SIG_DFL);
    signal(SIGPIPE, SIG_IGN);

    //AC_SET_DEFAULT_LOGGER(&base::OStreamLogger::StdoutLogger);

    netmgr::NetMgrMsgFactory factory;
    netmgr::GetGlobalMsgFactory()->RegisterFactory(&factory);
    QueueMsgFactory queueFactory;
    netmgr::GetGlobalMsgFactory()->RegisterFactory(&queueFactory);
   
    processMgr = new MsgQueueProcessManager;
    if( 0 != processMgr->Init(configFile))
    {
       printf("process mgr init error\n");
    }

    printf("process mgr run...\n");
    processMgr->Run();

    printf("process mgr close...\n");
    processMgr->Uninit();

    /*QueueProcessor app(1);
    #if( 0 != app.Open(configFile))
    #{
    #    printf("msg processor open error");
    #    return -1;
    #}

    #app.Run();

    #app.Close();
*/
    return 0;

}



