#include <queuemgr.h>
#include <queuemsg.h>
#include <ac/network/sockaddr.h>
#include <ac/log/log.h>
#include <ac/log/ostreamlogger.h>
#include <cassert>
#include <unistd.h>
#include <sstream>
#include <signal.h>
#include <ac/util/stringutil.h>
#include <stdio.h>
#include <libconfig/libconfig.h++>


//std::string PREELEMENT1 = "test";

void test_mgr(QueueMgr& mgr)
{
    RedisConnector* con= mgr.GetConnector();
    assert(con);
    int ret = mgr.LoadQueues(*con);
    AC_DEBUG("loadqueues errno:[%d]", ret);
    QueueMsg msg;
    QueueOption option;
    option.msgQueueName = "mytest";
}

int main(int ac, char*av[])
{
    if (ac < 2)
    {
        printf("%s cfg\n", av[0]);
        return -1;
    }
    signal(SIGPIPE, SIG_IGN);
    AC_SET_DEFAULT_LOGGER(&ac::OStreamLogger::StdoutLogger);
    QueueMgr mgr;
    try
    {
        libconfig::Config config; 
        config.readFile(av[1]);

        libconfig::Setting& setting = config.getRoot();
        if(0 != mgr.Open(setting))
        {
            AC_ERROR("open processor error");
            return -1;
        }
        test_mgr(mgr);

        
    }
    catch ( libconfig::SettingException & e ) 
    {
        printf("ConfigException: %s\n", e.getPath());
    }
    catch ( libconfig::ParseException & e ) 
    {
        printf("ParseException: %s in line %d\n", e.getError(), e.getLine());
    }
    catch ( libconfig::FileIOException & e ) 
    {
        printf("FileIOException\n");
    }
    catch ( libconfig::ConfigException & e ) 
    {
        printf("ConfigException\n");
    }
    catch ( std::exception & e ) 
    {
        printf("%s\n", e.what());
    }
}
