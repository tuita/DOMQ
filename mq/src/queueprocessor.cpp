#include "queueprocessor.h"
#include "queuemgr.h"
#include "util.h"
#include "role.h"
#include "redisconnector.h"
#include "queuemsg.h"
#include <uuid/uuid.h> 
#include <msg/msgqueueerrno.h>
#include <msg/msgqueuemsg.h>
#include <base/third/scope_guard.h>
#include <base/util/util.h>
#include <netmgr/connectionmgr.h>
#include <netmgr/factory.h>
#include <base/log/log.h>
#include <cassert>
#include <base/config/libconfig.h++>
#include <unistd.h>
#include <fcntl.h>


#define  REOPENCOUNT 100

const char* QueueCmd::HELP = "help";
const char* QueueCmd::EXIT = "exit";
const char* QueueCmd::EXITTHREAD = "exit thread";
const char* QueueCmd::STATUS = "status";
const char* QueueCmd::GETCLIENT = "get client";
const char* QueueCmd::ALLQUEUE = "all queue";
const char* QueueCmd::LOADMETA = "load meta";
const char* QueueCmd::RELOAD = "reload";
const char* QueueCmd::OPENDEBUG = "open debug";
const char* QueueCmd::CLOSEDEBUG = "close debug";
const char* QueueCmd::OPENNETINFO = "open netinfo";
const char* QueueCmd::CLOSENETINFO = "close netinfo";
const char* QueueCmd::OPENINFO = "open info";
const char* QueueCmd::CLOSEINFO = "close info";


int QueueProcessor::Open(const std::string& configFile)
{
    _configFile = configFile;
    if (_controlFd >=0)
    {
        int flags = fcntl(_controlFd, F_GETFL, 0);
        if (flags == -1)
        {
            return -1;
        }
        flags |= O_NONBLOCK;

        if (fcntl(_controlFd, F_SETFL, flags) != 0)
        {
            return -1;
        }
    }
    
    try
    {
        libconfig::Config config; 
        config.readFile(configFile.c_str());
        libconfig::Setting& setting = config.getRoot();
        return Open(setting);
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
    return -1;

}
int QueueProcessor::Open(const libconfig::Setting& setting)
{
    libconfig::Setting& app = setting["App"];
    LOG_INIT(app["Log"], (int)app["LogSize"], base::L_DEBUG);
    //SetLogger(app);
    /*if ( (bool)app["RuntimeDebug"] == false ) 
    {
        AC_SET_LOGGER(base::LP_DEBUG | base::LP_LOG_DEBUG , base::NullLogger::Instance());
    }
    else
    {
        AC_SET_LOGGER(base::LP_DEBUG | base::LP_LOG_DEBUG, GetLogger(DEFAULT_LOGGER));
    }

    if ( !app.exists("RuntimeNetInfo") || (bool)app["RuntimeNetInfo"] == false ) 
    {
        AC_SET_LOGGER(base::LP_AC_NET_INFO, base::NullLogger::Instance());
    }
    else
    {
        AC_SET_LOGGER(base::LP_AC_NET_INFO, GetLogger(DEFAULT_LOGGER));
    }

    if ( !app.exists("RuntimeInfo") || (bool)app["RuntimeInfo"] == false ) 
    {
        AC_SET_LOGGER(base::LP_INFO, base::NullLogger::Instance());
    }
    else
    {
        AC_SET_LOGGER(base::LP_INFO, GetLogger(DEFAULT_LOGGER));
    }*/
    LOG_OPEN();

    int threadCount = (int)app["ThreadCount"];
    _removeOnceCount = app.exists("RemoveExpireMsgCodeOnce") ? (int)app["RemoveExpireMsgCodeOnce"] : 1000;
    _popMsgBlockTimeUs = app.exists("PopMsgBlockTimeUs") ? (int)app["PopMsgBlockTimeUs"] : 1000;
    _sessionTime = app.exists("MsgCodeSessionTime") ? (int)app["MsgCodeSessionTime"] : 1000;
    _heartBeatMs = app.exists("HeartBeatMs") ? (int)app["HeartBeatMs"] : 1000;
    if(!_connMgr)
    {
        _connMgr = new netmgr::ConnectionMgr;
        if (0 != _connMgr->Open(setting["Network"]))
        {
            return -1;
        }
    }

    if(!_queueMgr)
    {
        _queueMgr = new QueueMgr;
    }

    if (0 != _queueMgr->Open(setting["Queue"]))
    {
        return -1;
    }
   
    Start(threadCount);
    return 0;
}

void QueueProcessor::Close()
{
    StopAll();
    LOG_DEBUG("queueprocessor close");
    _connMgr->Stop();
    LOG_CLOSE();
}

void QueueProcessor::StopThreads(size_t stopcount)
{
    for (size_t i=0; i<stopcount; ++i)
    {
        netmgr::LineMessage* msg = new netmgr::LineMessage;
        msg->line = QueueCmd::EXITTHREAD;
        _requestQueue.Push(msg);
    }
}

#define _PROCESS_MSG(MSGNAME, REQ_TYPE, RES_TYPE) if (MSGNAME == #REQ_TYPE)\
{\
    RES_TYPE res;\
    REQ_TYPE* req = dynamic_cast<REQ_TYPE*>(m);\
    res.context = req->context;\
    LOG_DEBUG("recv msgname:[%s], msgdata:[%s]", req->GetTypeName().c_str(), req->ToString().c_str());\
    int ret = ProcessMsg(*(req), res);\
    if(ret == 0)\
    {\
    LOG_DEBUG("send msgname:[%s], msgdata:[%s]", res.GetTypeName().c_str(), res.ToString().c_str());\
        _connMgr->PushMsg(&res);\
        continue;\
    }\
    else if (ret < 0)\
    {\
        break;\
    }\
}

void QueueProcessor::Run()
{
    _connMgr->Run();
    netmgr::Message* m;
    char buffer[256];
    while(true)
    {

         if( !(m=_connMgr->PopMsg(_popMsgBlockTimeUs)))
         {
             if (_controlFd>=0)
             {
                 int ret = ::recv(_controlFd, buffer, sizeof(buffer), 0);
                 if (ret == -1)
                 {
                     LOG_DEBUG("read control cmd, ret:%d", ret);
                 }
                 else
                 {
                     buffer[ret] = '\0';
                     LOG_INFO("socketpair_rcv: %s", buffer);
                     if(strcmp(buffer, "stop") == 0)
                     {
                         return;
                     }
                 }
             }
             RemoveExpiremsgQueueCode(time(NULL)-_sessionTime);
             continue;
         }
       
        base::ScopeGuard guard = base::MakeObjGuard(*netmgr::GetGlobalMsgFactory(), &netmgr::MappedMsgFactory::Destroy, m);
        const std::string& msgName = m->GetTypeName();
        LOG_DEBUG("recv msgname:[%s], msgdata:[%s]", m->GetTypeName().c_str(), m->ToString().c_str());
        _PROCESS_MSG(msgName, netmgr::LineMessage, netmgr::LineMessage)
        else 
        {
            guard.Dismiss();
            if(0 != _requestQueue.Push(m))
            {
                LOG_ERROR("push request to queue error");
            }
        }
    }
}

bool QueueProcessor::RunOnce()
{
    while(true)
    {
        netmgr::Message* m = NULL;
        if( 0!= _requestQueue.Pop(m, _heartBeatMs*1000))
        {
            _queueMgr->Ping();
            continue;
        }
        if (NULL == m)
        {
            LOG_DEBUG("pop msg null");
            continue;
        }
        BASE_ON_BLOCK_EXIT(&netmgr::MappedMsgFactory::Destroy, *netmgr::GetGlobalMsgFactory(), m);
        const std::string& msgName = m->GetTypeName();
        _PROCESS_MSG(msgName, GetMsgRequest, GetMsgResult)
        _PROCESS_MSG(msgName, SendMsgRequest, SendMsgResult)
        _PROCESS_MSG(msgName, OpenMsgQueueRequest, OpenMsgQueueResult)
        _PROCESS_MSG(msgName, CloseMsgQueueRequest, CloseMsgQueueResult)
        _PROCESS_MSG(msgName, netmgr::LineMessage, netmgr::LineMessage)
        else
        {
            LOG_ERROR("recv unknown msg:[%s]", m->GetTypeName().c_str());
        }
    }
    LOG_INFO("exit thread");
    return false;
}
#undef _PROCESS_MSG

int QueueProcessor::ProcessMsg(const OpenMsgQueueRequest& req, OpenMsgQueueResult& res)
{
    QueueOption option = {req.msgQueueName, req.option, req.duration, 0};
    int ret = _queueMgr->OpenQueue(option, QueueRole(req.role), req.create, req.context.peer.addr, req.clientName, res.msgQueueCode);
    switch (ret)
    {
    case MsgQueueErrorNo::SUCCESS:
        res.setErrNo(ret);
        break;
    case MsgQueueErrorNo::NO_EXIST:
        res.setErrNo(MsgQueueErrorNo::NO_EXIST);
        res.setErrMsg("msqeueue is not exist");
        break;

    default:
        res.setErrNo(ret);
        const char* errmsg = MsgQueueError(ret);
        res.setErrMsg(errmsg);
        break;
    }
    return 0;
}

int QueueProcessor::ProcessMsg(const CloseMsgQueueRequest& req, CloseMsgQueueResult& res)
{
    int ret = _queueMgr->CloseQueue(req.msgQueueCode);
    res.setErrNo(ret);
    if (MsgQueueErrorNo::SUCCESS != ret)
    {
        res.setErrMsg(MsgQueueError(ret));
    }
    return 0;
}

int QueueProcessor::ProcessMsg(const GetMsgRequest& req, GetMsgResult& res)
{
    QueueMsg msg;
    int ret = _queueMgr->PopMsg(req.msgQueueCode, msg);
    if (MsgQueueErrorNo::SUCCESS != ret)
    {
        res.setErrNo(ret);
        res.setErrMsg(MsgQueueError(ret));
    }
    else
    {
        res.setErrNo(MsgQueueErrorNo::SUCCESS);
        res.data = msg.data;
    }
    return 0;
}

int QueueProcessor::ProcessMsg(const SendMsgRequest& req, SendMsgResult& res)
{
    QueueMsg msg;
    msg.data = req.msgData;
    int ret = _queueMgr->PushMsg(req.msgQueueCode, msg);
    if (MsgQueueErrorNo::SUCCESS != ret)
    {
        res.setErrNo(ret) ;
        res.setErrMsg(MsgQueueError(ret));
    }
    else
    {
        res.setErrNo(MsgQueueErrorNo::SUCCESS);
    }
    return 0;
}

int QueueProcessor::ProcessMsg(const netmgr::LineMessage& req, netmgr::LineMessage& res)
{
    LOG_DEBUG("receive line msg:[%s]", base::Escape(req.line).c_str());
    const std::string& line = req.line;
    if (0 == strncasecmp(line.c_str(), QueueCmd::HELP, strlen(QueueCmd::HELP)))
    {
        std::stringstream st;
        st  << "cmd:" << std::endl
            << QueueCmd::HELP       << std::endl
            << QueueCmd::OPENDEBUG       << std::endl
            << QueueCmd::CLOSEDEBUG       << std::endl
            << QueueCmd::OPENNETINFO       << std::endl
            << QueueCmd::CLOSENETINFO       << std::endl
            << QueueCmd::OPENINFO       << std::endl
            << QueueCmd::CLOSEINFO       << std::endl
            << QueueCmd::EXIT       << std::endl
            << QueueCmd::HELP       << std::endl
            << QueueCmd::STATUS     << std::endl
            << QueueCmd::ALLQUEUE   << std::endl
            << QueueCmd::LOADMETA   << std::endl
            << QueueCmd::RELOAD     << std::endl;
        res.line = st.str();
        return 0;
    }
    else if (0 == strncasecmp(line.c_str(), QueueCmd::OPENDEBUG, strlen(QueueCmd::OPENDEBUG)))
    {
        res.line = "open debug succ\r\n";
        return 0;
    }
    else if (0 == strncasecmp(line.c_str(), QueueCmd::CLOSEDEBUG, strlen(QueueCmd::CLOSEDEBUG)))
    {
        res.line = "close debug succ\r\n";
        return 0;
    }
    else if (0 == strncasecmp(line.c_str(), QueueCmd::OPENNETINFO, strlen(QueueCmd::OPENNETINFO)))
    {
        res.line = "open netinfo succ\r\n";
        return 0;
    }
    else if (0 == strncasecmp(line.c_str(), QueueCmd::CLOSENETINFO, strlen(QueueCmd::CLOSENETINFO)))
    {
        res.line = "close netinfo succ\r\n";
        return 0;
    }
    else if (0 == strncasecmp(line.c_str(), QueueCmd::OPENINFO, strlen(QueueCmd::OPENINFO)))
    {
        res.line = "open info succ\r\n";
        return 0;
    }
    else if (0 == strncasecmp(line.c_str(), QueueCmd::CLOSEINFO, strlen(QueueCmd::CLOSEINFO)))
    {
        res.line = "close info succ\r\n";
        return 0;
    }
    else if (0 == strncasecmp(line.c_str(), QueueCmd::EXITTHREAD, strlen(QueueCmd::EXITTHREAD)))
    {
        res.line = "exitthread succ\r\n";
        return -1;
    }
    else if (0 == strncasecmp(line.c_str(), QueueCmd::EXIT, strlen(QueueCmd::EXIT)))
    {
        res.line = "exit succ\r\n";
        return -1;
    }
    else if (0 == strncasecmp(line.c_str(), QueueCmd::STATUS, strlen(QueueCmd::STATUS)))
    {
        std::stringstream st;
        st << "thread:" << livethreads.size() << std::endl; 
        res.line = st.str();
        return 0;
    }
    else if (0 == strncasecmp(line.c_str(), QueueCmd::LOADMETA, strlen(QueueCmd::LOADMETA)))
    {
        int ret = _queueMgr->LoadQueueMeta();
        res.line = ret == 0 ? "succ\r\n" : "fail\r\n";
        return 0;
    }
    else if (0 == strncasecmp(line.c_str(), QueueCmd::RELOAD, strlen(QueueCmd::RELOAD)))
    {
        StopAll();
        Open(_configFile);
        res.line = "reload succ\r\n";
        return 0;
    }
    else if (0 == strncasecmp(line.c_str(), QueueCmd::GETCLIENT, strlen(QueueCmd::GETCLIENT)))
    {
        size_t last = 0;
        const std::string del = " \t,;\r\n";
        std::string data = line.substr(strlen(QueueCmd::GETCLIENT));
        size_t index= data.find_first_of(del,last);
        std::vector<std::string> msgCodes;
        while (index!=std::string::npos)
        {
            std::string msgCode = data.substr(last,index-last);
            if (index > last)
            {
                msgCodes.push_back(msgCode);
            }
            last=index+1;
            index=data.find_first_of(del, last);
        }
        if (last < (data.size()-1))
        {
            msgCodes.push_back(data.substr(last));
        }
        std::vector<QueueClient> clients; 
        _queueMgr->GetClients(msgCodes, clients);
        std::stringstream st ;
        st << "clientInfo:" << std::endl;
        for (std::vector<QueueClient>::const_iterator it = clients.begin(); it != clients.end(); ++it)
        {
            st << it->ToString() << std::endl;
        }

        res.line = st.str();
        return 0;
    }
    
    else if (0 == strncasecmp(line.c_str(), QueueCmd::ALLQUEUE, strlen(QueueCmd::ALLQUEUE)))
    {
        std::vector<Queue> queues;
        if(0 != _queueMgr->GetAllQueue(queues))
        {
            res.line = "getallqueue error\r\n";
        }
        else
        {
            std::stringstream st;
            st << "meta queue info:" << std::endl;
            for (std::vector<Queue>::const_iterator it = queues.begin(); it!=queues.end(); ++it)
            {
                st << it->ToString() << std::endl;
            }
            res.line = st.str();
        }
        return 0;
    }
    else
    {
        res.line = "unknow cmd\r\n";
        return 0;
    }
    return 0;
}

void QueueProcessor::RemoveExpiremsgQueueCode(time_t expire)
{
    _queueMgr->RemoveExpireMsgQueueCode(expire);
}

