#ifndef _QUEUE_PROCESSOR_H__
#define _QUEUE_PROCESSOR_H__

#include <string>
#include <list>
#include <map>
#include <base/thread/threadpool.h>
#include <base/net/sockaddr.h>
#include <base/thread/mutex.h>
#include <base/util/objectqueue.h>
#include "queue.h"
#include "role.h"

namespace libconfig
{
    class Setting;
}

namespace netmgr
{
    class Message;
    class ConnectionMgr;
    class LineMessage;
}

class QueueOption;
class QueueMsg;
class RedisConnector;
class OpenMsgQueueRequest;
class OpenMsgQueueResult;
class CloseMsgQueueRequest;
class CloseMsgQueueResult;
class SendMsgRequest;
class SendMsgResult;
class GetMsgRequest;
class GetMsgResult;

class QueueMgr;
struct QueueCmd
{
    const static char* HELP;
    const static char* EXIT;
    const static char* EXITTHREAD;
    const static char* GETCLIENT;
    const static char* STATUS;
    const static char* ALLQUEUE;
    const static char* LOADMETA;
    const static char* RELOAD;
    const static char* OPENDEBUG;
    const static char* CLOSEDEBUG;
    const static char* OPENNETINFO;
    const static char* CLOSENETINFO;
    const static char* OPENINFO;
    const static char* CLOSEINFO;
};

class QueueProcessor: public base::ThreadPool
{
public:
    QueueProcessor(int controlFd=-1):_connMgr(NULL), _queueMgr(NULL), _controlFd(controlFd){}
    int Open(const std::string& config);
    int Open(const libconfig::Setting& setting);
    void Close();
    void Run();

protected:

    bool RunOnce();

    int ProcessMsg(const OpenMsgQueueRequest& req, OpenMsgQueueResult& res);
    int ProcessMsg(const CloseMsgQueueRequest& req, CloseMsgQueueResult& res);
    int ProcessMsg(const SendMsgRequest& req, SendMsgResult& res);
    int ProcessMsg(const GetMsgRequest& req, GetMsgResult& res);
    int ProcessMsg(const netmgr::LineMessage& req, netmgr::LineMessage& res);

    void RemoveExpiremsgQueueCode(time_t expire);

    void StopThreads(size_t stopcount);

private:
    netmgr::ConnectionMgr* _connMgr;
    QueueMgr* _queueMgr;

    base::SyncObjectQueue<netmgr::Message*> _requestQueue;

    std::string _configFile;
    int _popMsgBlockTimeUs;
    int _removeOnceCount;
    int _sessionTime;
    int _heartBeatMs;

    int _controlFd;

};
#endif

