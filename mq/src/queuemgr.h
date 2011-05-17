#ifndef _QUEUE_MGR_H__
#define _QUEUE_MGR_H__

#include <string>
#include <list>
#include <vector>
#include <map>
#include <base/thread/threadpool.h>
#include <base/thread/threadlocal.h>
#include <base/net/sockaddr.h>
#include <base/thread/mutex.h>
#include <base/util/objectqueue.h>
#include "queue.h"
#include "role.h"
#include "redisconnector.h"

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

class QueueMetaData
{
public:
    static std::string allQueueName ;

    static std::string GetQueueMetaName(const std::string& queueName)
    {
        return "__meta_queue_" + queueName;
    }

    static std::string GetQueueMsgName(const std::string& queueName)
    {
        return "_queue_" + queueName;
    }

    static std::string GetSubQueueMetaName(const std::string queueName)
    {
        return "__meta_sub_" + queueName;
    }

    static std::string GetQueueSubName(const std::string& queuename, const std::string& clientName)
    {
        return "_subqueue_" + queuename + "-" + clientName;
    }

};

class QueueMgr
{
public:
    QueueMgr();
    int Open(const libconfig::Setting& setting);
    void Close();
    int OpenQueue(const QueueOption& option, QueueRole role, int flag, const std::string& addr, const std::string& clientName, std::string& msgQueueCode);
    int CloseQueue(const std::string& name);
    int LoadQueueMeta();
    int PushMsg(const std::string& msgQueueCode, const QueueMsg& msg);
    int PopMsg(const std::string& msgQueueCode, QueueMsg& msg);
    int GetAllQueue(std::vector<Queue>& queues);
    void RemoveExpireMsgQueueCode(time_t expireTime);
    void RemoveExpireQueue(time_t expireTime);
    void GetClients(const std::vector<std::string> msgCodes, std::vector<QueueClient>& clientInfos) const;
    void Ping();
protected:
    
    int GetQueueName(const std::string& msgQueueCode, std::string& queueName, std::string& clientName);
    int LoadQueues(RedisConnector& con);
    int LoadQueue(RedisConnector& con, const std::string& queueName);

    base::SockAddress GetQueueAddr(const std::string& /*queueName*/){ return _redisAddr;}

    RedisConnector* GetConnector();
    static void ReleaseConnector(void* con);

    std::string GenMsgQueueCode();
    
    int CreatePublisher(RedisConnector& con, const QueueOption& option, int create, const std::string& addr);
    int CreateSubscriber(RedisConnector& con, const QueueOption& option,const std::string& addr, const std::string& clientName);

    int GetQueueValue(Queue& queue, const RedisReply& reply) const;
    int GetSubQueueValue(std::string& name, time_t& lastVisit, const RedisReply& reply) const;

private:
    std::list<RedisConnector*> _redisConns;

    base::SockAddress _redisAddr;
    int _msTimeout;
    std::string _pwd;

    int _popMsgBlockTimeUs;
    int _removeOnceCount;
    int _sessionTime;

    base::ThreadLocal _redisConn ;

    base::ThreadMutex _redisMutex;
    base::ThreadMutex _rwMutex;

    typedef std::map<std::string, Queue> NAME2QUEUE;
    NAME2QUEUE _name2Queue; //名字到队列的映射， cache数据

    struct tagMsgCodeSeqItem
    {
        int time;
        std::string msgQueueCode;
    };
    typedef std::list<tagMsgCodeSeqItem> MSGCODESEQUENCE;
    struct tagMsgCode2QueueItem
    {
        std::string queueName;
        std::string clientName;
        MSGCODESEQUENCE::iterator pos;
    };
    typedef std::map<std::string, tagMsgCode2QueueItem> QUEUECODE2QUEUENAME;
    typedef std::map< std::pair<std::string , std::string>, std::string > CLIENT2MSGCODE;


    QUEUECODE2QUEUENAME _msgQueueCode2QueueName;
    MSGCODESEQUENCE _msgCodeSequence; //记录msgQueueCode的时间, 清除长时间未用的部分
    CLIENT2MSGCODE  _client2MsgCode; //根据clientname和queuename查找msgQueueCode

    

    typedef std::map<std::string, time_t> CLIENTVISIT;  
    typedef std::map<std::string, time_t> QUEUEVISIT;

};
#endif
