#include <netmgr/connectionmgr.h>
#include <base/config/libconfig.h++>
#include <base/log/log.h>
#include <base/util/util.h>
#include <netmgr/factory.h>
#include <netmgr/msgfactory.h>
#include <addmsg.h>
#include <base/third/scope_guard.h>
#include <base/thread/thread.h>
#include <base/thread/threadpool.h>
#include "msgfactory.h"

void processMsg(const netmgr::LineMessage& req, netmgr::LineMessage& res)
{
    res.context = req.context;
    res.serialNo = req.serialNo;
    res.line = req.line;
    LOG_DEBUG("req line:[%s], res line:%s", base::Escape(req.line).c_str(), base::Escape(res.line).c_str());
}

void processMsg(const AddRequest& req, AddResult& res)
{
    res.context.peer = req.context.peer;
    res.serialNo = req.serialNo;
    res.error = 0 ; 
    res.sum = req.param1 + req.param2;
    LOG_DEBUG("param1:[%u], param2:[%u], sum:[%u]", req.param1, req.param2, res.sum);
}

void processMsg(netmgr::ConnectionMgr& mgr)
{
    while(true)
    {
        netmgr::Message* m = mgr.PopMsg(true);
        if (NULL == m)
        {
            LOG_ERROR("popmsg error");
            continue;
        }
        BASE_BLOCK_GUARD(&netmgr::MappedMsgFactory::Destroy, *netmgr::GetGlobalMsgFactory(), m);
        assert(m);
        LOG_DEBUG("recv msg:%s, content:%s", m->GetTypeName().c_str(), m->ToString().c_str());
        std::string msgName = m->GetTypeName();
        if (msgName == "netmgr::LineMessage")
        {
            netmgr::LineMessage* req = dynamic_cast<netmgr::LineMessage*>(m);
            netmgr::LineMessage res;
            processMsg(*req, res);
            int ret = mgr.PushMsg(&res);
            LOG_DEBUG("send msg ret:%d", ret);
        }
        else if (msgName == "AddRequest")
        {
            AddRequest* req = dynamic_cast<AddRequest*>(m);
            AddResult res;
            processMsg(*req, res);
            int ret = mgr.PushMsg(&res);
            LOG_DEBUG("send msg ret:%d", ret);
        }
        else
        {
            LOG_ERROR("recv unknown msg:[%s]", m->GetTypeName().c_str());
        }
    }
}

class AddThreadPool:public base::ThreadPool
{
public:
    void SetConntionMgr(netmgr::ConnectionMgr& conMgr)
    {
        _conMgr = &conMgr;
    }
protected:
    bool RunOnce()
    {
        processMsg(*_conMgr);
        return true;
    }
private:
    netmgr::ConnectionMgr* _conMgr;

};

int main(int ac, char*av[])
{
    if (ac != 2)
    {
        printf("%s configfile\n", av[0]);
        return -1;
    }

    AC_SET_DEFAULT_LOGGER(&base::OStreamLogger::StdoutLogger);
    try
    {
        libconfig::Config config; 
        config.readFile(av[1]);
        libconfig::Setting& setting = config.getRoot()["App"];
        netmgr::AcnMsgFactory acnFactory;
        netmgr::GetGlobalMsgFactory()->RegisterFactory(&acnFactory);
        MsgFactory factory;
        netmgr::GetGlobalMsgFactory()->RegisterFactory(&factory);

        netmgr::ConnectionMgr mgr; 
        mgr.Open(setting);
        AddThreadPool threadPool;
        threadPool.SetConntionMgr(mgr);
        threadPool.Start(10);

        mgr.Run(true);
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
    return 0;
}
