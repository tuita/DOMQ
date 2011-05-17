#include "queuemgr.h"
#include "redisconnector.h"
#include "queuemsg.h"
#include <base/log/log.h>
#include <msg/msgqueueerrno.h>
#include <msg/msgqueuemsg.h>
#include <base/third/scope_guard.h>
#include <base/util/util.h>
#include <netmgr/connectionmgr.h>
#include <netmgr/factory.h>
#include <uuid/uuid.h> 
#include <cassert>
#include <base/config/libconfig.h++>
#include <unistd.h>
#include <list>
#include <algorithm>

#define  REOPENCOUNT 100
#define  _CHECK_REPLY(_REDISCON, _REPLY_, _RIGHT_TYPE, _ERRNO_, _ISCHECK_NILL, _GETALLREPLYRETURN, _FORMAT, ...)  do{ \
    int ret = 0; \
    if (_REPLY_ == NULL){ LOG_ERROR("load error from redis:[%s], reply is null, errno:[%d], errmsg[%s], redis cmd:"#_FORMAT, _redisAddr.ToString().c_str(), _REDISCON->GetLastErrNo(), _REDISCON->GetLastError(), __VA_ARGS__);ret= _ERRNO_;} \
        else if (_ISCHECK_NILL && REDIS_REPLY_NIL == _REPLY_->type){ LOG_DEBUG("load error from redis:[%s], reply is REDIS_REPLY_NIL, redis cmd:"#_FORMAT, _redisAddr.ToString().c_str(), __VA_ARGS__);ret= MsgQueueErrorNo::NO_EXIST;}\
        else if (REDIS_REPLY_NIL != _REPLY_->type && _RIGHT_TYPE != _REPLY_->type){ if( REDIS_REPLY_ERROR == _REPLY_->type){LOG_ERROR("type should be :[%d], error type:[%d], errmsg:[%s], redis cmd:"#_FORMAT, _RIGHT_TYPE, _REPLY_->type, _REPLY_->str, __VA_ARGS__);} else { LOG_ERROR("type should be :[%d], error type:[%d], redis cmd:"#_FORMAT, _RIGHT_TYPE, _REPLY_->type, __VA_ARGS__);} ret=_ERRNO_;}\
        if(ret != 0 && _GETALLREPLYRETURN >0) \
        {\
        RedisReply*  _tmpReply = NULL;\
        for(int i=0; i< int(_GETALLREPLYRETURN); ++i)\
            {\
            if(REDIS_OK != _REDISCON->GetReply(_tmpReply)) break;\
            BASE_BLOCK_GUARD(&RedisConnector::FreeReply, *_REDISCON, _tmpReply);\
            }\
        }\
        if(ret != 0) return ret;\
    }while(false)\


std::string QueueMetaData::allQueueName;

static std::list<std::string>* GetClientsBuf()
{
    static __thread  std::list<std::string>* gListClient ;
    if (gListClient == NULL)
    {
        gListClient = new std::list<std::string>;
    }
    return gListClient;
}
QueueMgr::QueueMgr():_redisConn(&QueueMgr::ReleaseConnector)
{

}

int QueueMgr::Open(const libconfig::Setting& setting)
{
    QueueMetaData::allQueueName = "__meta_queuenames";
    _redisAddr.SetAddr(setting["RedisAddr"]);
    _msTimeout = (int)setting["Timeout"];
    _pwd = (const char*)setting["Password"];

    _removeOnceCount = setting.exists("RemoveExpireMsgCodeOnce") ? (int)setting["RemoveExpireMsgCodeOnce"] : 1000;

    if(0 != LoadQueueMeta())
    {
        LOG_ERROR("load meta data error, redis:[%s]", _redisAddr.ToString().c_str());
        return -1;
    }
    return 0;
}

void QueueMgr::Close()
{
}

int QueueMgr::CreatePublisher(RedisConnector& con, const QueueOption& option, int create, const std::string& /*addr*/)
{
    RedisReply* reply = con.Command("%s %s %s", RedisCmd::SISMEMBER.c_str(), QueueMetaData::allQueueName.c_str(), option.msgQueueName.c_str());
    BASE_BLOCK_GUARD(&RedisConnector::FreeReply, con, reply);
    _CHECK_REPLY((&con), reply, REDIS_REPLY_INTEGER, -1, true, 0, "[%s %s %s]", RedisCmd::SISMEMBER.c_str(), QueueMetaData::allQueueName.c_str(), option.msgQueueName.c_str());
    if (reply->integer == 0)
    {
        if (create&0x1)
        {
            con.AppendCommand("%s %s %s", RedisCmd::SADD.c_str(), QueueMetaData::allQueueName.c_str(), option.msgQueueName.c_str());
            con.AppendCommand("%s %s %s %s %s %d %s %d %s %d %s %d", RedisCmd::HMSET.c_str(), QueueMetaData::GetQueueMetaName(option.msgQueueName).c_str(),
                "name", option.msgQueueName.c_str(), 
                "duration", option.duration, 
                "maxcount", option.maxCount, 
                "type", option.msgType,
                "lastvisit", time(NULL));

            for (size_t i=0; i<2; ++i)
            {
                RedisReply* reply  = NULL;
                int ret = con.GetReply(reply);
                if (REDIS_OK != ret)
                {
                    LOG_ERROR("create publish error");
                    return MsgQueueErrorNo::INTERNAL_ERROR; 
                }
                BASE_BLOCK_GUARD(&RedisConnector::FreeReply, con, reply);
            }
        }
        else
        {
            LOG_ERROR("queue:[%s] is not exist", option.msgQueueName.c_str());
            return MsgQueueErrorNo::NO_EXIST;
        }
    }
    return MsgQueueErrorNo::SUCCESS;
}

int QueueMgr::CreateSubscriber(RedisConnector& con, const QueueOption& option,const std::string& /*addr*/, const std::string& clientName)
{
    //检查订阅队列是否存在
    RedisReply* reply = NULL;
    con.AppendCommand("%s %s %s", RedisCmd::SISMEMBER.c_str(), QueueMetaData::allQueueName.c_str(), option.msgQueueName.c_str()); //检查queuename是否存在
    con.AppendCommand("%s %s", RedisCmd::HGETALL.c_str(), QueueMetaData::GetQueueMetaName(option.msgQueueName).c_str());

    int ret = con.GetReply(reply);
    if (REDIS_OK != ret)
    {
        LOG_ERROR("check , rediscmd:[%s], [%s], [%s] error, errno:[%d]", RedisCmd::SISMEMBER.c_str(), QueueMetaData::allQueueName.c_str(), option.msgQueueName.c_str(), ret);
        return MsgQueueErrorNo::INTERNAL_ERROR;

    }
    else
    {
        BASE_BLOCK_GUARD(&RedisConnector::FreeReply, con, reply);
        _CHECK_REPLY((&con), reply, REDIS_REPLY_INTEGER, -1, true, 1, "[%s %s %s]", RedisCmd::SISMEMBER.c_str(), QueueMetaData::allQueueName.c_str(), option.msgQueueName.c_str());
        if(reply->integer != 1)
        {
            if( REDIS_OK == con.GetReply(reply))
            {
                BASE_BLOCK_GUARD(&RedisConnector::FreeReply, con, reply);
            }
            LOG_DEBUG("redis cmd:[%s %s %s]", RedisCmd::SISMEMBER.c_str(), QueueMetaData::allQueueName.c_str(), option.msgQueueName.c_str());
            return MsgQueueErrorNo::NO_EXIST;
        }
    }

    if (REDIS_OK != con.GetReply(reply))
    {
        LOG_ERROR("check , rediscmd:[%s], [%s]error", RedisCmd::HGETALL.c_str(), QueueMetaData::GetQueueMetaName(option.msgQueueName).c_str());
        return MsgQueueErrorNo::INTERNAL_ERROR;

    }

    //获取已有队列远信息
    Queue queue;
    {
        BASE_BLOCK_GUARD(&RedisConnector::FreeReply, con, reply);
        _CHECK_REPLY((&con), reply, REDIS_REPLY_ARRAY, -1, true, 0, "[%s %s]", RedisCmd::HGETALL.c_str(), QueueMetaData::GetQueueMetaName(option.msgQueueName).c_str());

        if (0 != GetQueueValue(queue, *reply))
        {
            LOG_ERROR("queue:[%s] meta is error", option.msgQueueName.c_str());
            return MsgQueueErrorNo::INTERNAL_ERROR;
        }
        //todo 加载queue   
    }

    if(queue.option.msgType != option.msgType)
    {
        LOG_ERROR("queue:[%s], msgType:[%d], subcriber msgType:[%d]", option.msgQueueName.c_str(), queue.option.msgType, option.msgType);
        return MsgQueueErrorNo::MSGTYPE_ERROR;
    }

    if (option.msgType == PEER2PEER)
    {
        return MsgQueueErrorNo::SUCCESS;
    }


    {
        reply = con.Command("%s %s %s", RedisCmd::HEXISTS.c_str(), QueueMetaData::GetSubQueueMetaName(option.msgQueueName).c_str(), clientName.c_str());
        _CHECK_REPLY((&con), reply, REDIS_REPLY_INTEGER, -1, true, 0, "[%s %s %s]", RedisCmd::HEXISTS.c_str(), QueueMetaData::GetSubQueueMetaName(option.msgQueueName).c_str(), clientName.c_str());
        BASE_BLOCK_GUARD(&RedisConnector::FreeReply, con, reply);
        if (reply->integer == 1) //create
        {
            return MsgQueueErrorNo::SUCCESS;
        }
    }

    {
        reply = con.Command("%s %s %s %zd", RedisCmd::HMSET.c_str(), QueueMetaData::GetSubQueueMetaName(option.msgQueueName).c_str(), clientName.c_str(), time(NULL));
        _CHECK_REPLY((&con), reply, REDIS_REPLY_STATUS, -1, true, 0, "[%s %s %s %ld]", RedisCmd::HMSET.c_str(), QueueMetaData::GetSubQueueMetaName(option.msgQueueName).c_str(), clientName.c_str(), time(NULL));
        BASE_BLOCK_GUARD(&RedisConnector::FreeReply, con, reply);
        base::MutexGuard lock(&_rwMutex);
        assert(0 == lock.Lock());
        Queue::SubQueue subQueue = {clientName, 0, time(NULL)};
        _name2Queue[option.msgQueueName].clients.push_back(subQueue);
    }


    return MsgQueueErrorNo::SUCCESS;
}

int QueueMgr::OpenQueue(const QueueOption& option, QueueRole role, int create, const std::string& addr, const std::string& clientName, std::string& msgQueueCode)
{
    RedisConnector* con = GetConnector();
    assert(con);
    int ret = 0 ;
    if (PUBLISHER == role)
    {
        ret = CreatePublisher(*con, option, create, addr);
    }
    else if (SUBSCRIBER == role)
    {
        ret = CreateSubscriber(*con, option, addr, clientName);
    }
    else
    {
        assert("role type is error");      
    };
    if (MsgQueueErrorNo::SUCCESS == ret)
    {
        base::MutexGuard lock(&_rwMutex);
        lock.Lock();
        msgQueueCode = GenMsgQueueCode();
        tagMsgCodeSeqItem seqItem;
        time_t now = time(NULL);
        seqItem.time = now;
        seqItem.msgQueueCode = msgQueueCode;
        _msgCodeSequence.push_front(seqItem);
        tagMsgCode2QueueItem& item = _msgQueueCode2QueueName[msgQueueCode];
        item.queueName = option.msgQueueName;
        item.clientName = clientName;
        item.pos = _msgCodeSequence.begin();

        NAME2QUEUE::iterator itQueue = _name2Queue.find(option.msgQueueName);
        if (itQueue == _name2Queue.end())
        {
            Queue& queue = _name2Queue[option.msgQueueName];
            queue.option = option;
            queue.msgCount = 0 ; 
            queue.redisAddr = _redisAddr;
            queue.lastVisit = now;
        }
        else
        {
            _name2Queue[option.msgQueueName].lastVisit = now;
        }

        _client2MsgCode[std::make_pair(clientName, option.msgQueueName)] = msgQueueCode;
    }

    return ret;
}


int QueueMgr::CloseQueue(const std::string& msgQueueCode)
{
    base::MutexGuard lock(&_rwMutex);
    lock.Lock();
    QUEUECODE2QUEUENAME::iterator it = _msgQueueCode2QueueName.find(msgQueueCode);
    if (it == _msgQueueCode2QueueName.end())
    {
        return MsgQueueErrorNo::QUEUECODE_NOEXIST;
    }
    _client2MsgCode.erase(std::make_pair(it->second.clientName, it->second.queueName));
    _msgCodeSequence.erase(it->second.pos);
    _msgQueueCode2QueueName.erase(it);

    return MsgQueueErrorNo::SUCCESS;
}

int QueueMgr::LoadQueueMeta()
{
    RedisConnector* con = GetConnector();
    assert(con);
    return LoadQueues(*con);
}


int QueueMgr::GetQueueName(const std::string& msgQueueCode, std::string& queueName, std::string& clientName)
{
    size_t now = time(NULL);

    QUEUECODE2QUEUENAME::iterator it = _msgQueueCode2QueueName.find(msgQueueCode);
    if (it == _msgQueueCode2QueueName.end())
    {
        return MsgQueueErrorNo::QUEUECODE_NOEXIST;
    }

    queueName = it->second.queueName;
    clientName = it->second.clientName;
    NAME2QUEUE::iterator itQueue = _name2Queue.find(queueName);
    if (itQueue == _name2Queue.end())
    {
        return MsgQueueErrorNo::NO_EXIST;
    }
    itQueue->second.lastVisit = now;
    tagMsgCode2QueueItem& item = it->second;
    item.pos->time = now;
    if (item.pos!=_msgCodeSequence.begin())
    {
        MSGCODESEQUENCE::iterator nextIter = item.pos;
        ++nextIter;
        _msgCodeSequence.splice(_msgCodeSequence.begin(), _msgCodeSequence, item.pos, nextIter);
        item.pos = _msgCodeSequence.begin();
    }

    LOG_DEBUG("get queuename, msgQueueCode:[%s], clientName:[%s], msgQueueName:[%s], time:[%d]", msgQueueCode.c_str(), it->second.clientName.c_str(), queueName.c_str(), _msgCodeSequence.front().time);
    return 0;
}

int QueueMgr::GetQueueValue(Queue& queue, const RedisReply& reply) const
{
    size_t count= 0 ;
    for(size_t i=0; i<reply.elements; i+=2)
    {
        const RedisReply* curReply = reply.element[i];
        const RedisReply* nextTmp = reply.element[1+i];
        if (0 == strncmp(curReply->str, "name", curReply->len))
        {
            ++count;
            queue.option.msgQueueName.assign(nextTmp->str, nextTmp->len);
        }
        else if (0 == strncmp(curReply->str, "duration", curReply->len))
        {
            ++count;
            queue.option.duration = atol(nextTmp->str);
        }
        else if (0 == strncmp(curReply->str, "maxcount", curReply->len))
        {
            ++count;
            queue.option.maxCount = atol(nextTmp->str);
        }
        else if (0 == strncmp(curReply->str, "type", curReply->len))
        {
            ++count;
            queue.option.msgType = atol(nextTmp->str);
        }
        else if (0 == strncmp(curReply->str, "lastvisit", curReply->len))
        {
            ++count;
            queue.lastVisit = atol(nextTmp->str);
        }
        else
        {
            LOG_WARN("unknown field:%s", curReply->str);
        }
    }
    if(count < 5)
    {
        return -1;
    }
    return 0;
}


int QueueMgr::LoadQueues(RedisConnector& con)
{
    //获取所有队列的元数据信息
    RedisReply* reply = con.Command("%s %s", RedisCmd::SMEMBERS.c_str(), QueueMetaData::allQueueName.c_str());
    BASE_BLOCK_GUARD(&RedisConnector::FreeReply, con, reply);
    _CHECK_REPLY((&con), reply, REDIS_REPLY_ARRAY, -1, false, 0, "%s %s", RedisCmd::SMEMBERS.c_str(), QueueMetaData::allQueueName.c_str());


    if (reply->type == REDIS_REPLY_NIL)
    {
        LOG_INFO("have not meta info, because of no queues, redis cmd:[%s %s]", RedisCmd::SMEMBERS.c_str(), QueueMetaData::allQueueName.c_str());
        return 0;
    }
    /*_name2Queue.clear();*/
    NAME2QUEUE name2QueueTmp; 
    for(size_t i=0; i<reply->elements; ++i)
    {
        //获取每个队列的元数据信息
        RedisReply* tmp = reply->element[i];
        _CHECK_REPLY((&con), tmp, REDIS_REPLY_STRING, -1, true, 0, "[%s %s]", RedisCmd::SMEMBERS.c_str(), QueueMetaData::allQueueName.c_str());

        std::string queueMetaName = QueueMetaData::GetQueueMetaName(tmp->str);
        std::string queueMsgName = QueueMetaData::GetQueueMsgName(tmp->str);

        LOG_DEBUG("get queue meta info, queue metaname:[%s], queuemsgname:[%s]", queueMetaName.c_str(), queueMsgName.c_str());

        con.AppendCommand("%s %s", RedisCmd::HGETALL.c_str(), queueMetaName.c_str());
        con.AppendCommand("%s %s", RedisCmd::LLEN.c_str(), queueMsgName.c_str());

        RedisReply* allFieldReply;
        if (REDIS_OK != con.GetReply(allFieldReply))
        {
            LOG_ERROR("get queue[%s] meta data error, redis cmd:[%s %s]", queueMetaName.c_str(), RedisCmd::HGETALL.c_str(), queueMetaName.c_str());
            return -1;
        }
        BASE_BLOCK_GUARD(&RedisConnector::FreeReply, con, allFieldReply);
        _CHECK_REPLY((&con), allFieldReply, REDIS_REPLY_ARRAY, -1, true, 1, "[%s %s]", RedisCmd::HGETALL.c_str(), queueMetaName.c_str());

        RedisReply* queueCountReply;
        if (REDIS_OK != con.GetReply(queueCountReply))
        {
            LOG_ERROR("get queue[%s] count error, [%s %s]", queueMsgName.c_str(), RedisCmd::LLEN.c_str(), queueMsgName.c_str());
            return -1;
        }
        BASE_BLOCK_GUARD(&RedisConnector::FreeReply, con, queueCountReply);
        _CHECK_REPLY((&con), queueCountReply, REDIS_REPLY_INTEGER, -1, true, 0, "[%s %s]", RedisCmd::LLEN.c_str(), queueMsgName.c_str());

        Queue& queue = name2QueueTmp[tmp->str];
        if (0 != GetQueueValue(queue, *allFieldReply))
        {
            LOG_ERROR("queue:[%s] meta info error", tmp->str);
            return -1;
        }
        queue.msgCount = queueCountReply->integer;
        if(queue.option.msgType == SUB2PUB)//获取每个子队列中的元数据和消息
        {
            std::string subQueueMetaName = QueueMetaData::GetSubQueueMetaName(queue.option.msgQueueName);
            con.AppendCommand("%s %s", RedisCmd::HGETALL.c_str(), subQueueMetaName.c_str());
            RedisReply* subReply = NULL;
            if(REDIS_OK != con.GetReply(subReply))
            {
                LOG_ERROR("get submsgqueue:[%s] error, redis cmd:[%s %s]", queue.option.msgQueueName.c_str(), RedisCmd::HGETALL.c_str(), subQueueMetaName.c_str());
                return -1;
            }
            BASE_BLOCK_GUARD(&RedisConnector::FreeReply, con, subReply);
            _CHECK_REPLY((&con), subReply, REDIS_REPLY_ARRAY, -1, false, 0, "[%s %s]", RedisCmd::HGETALL.c_str(), subQueueMetaName.c_str());
            if (subReply->type == REDIS_REPLY_NIL)
            {
                continue;
            }
            queue.clients.resize(subReply->elements/2);
            for (size_t j=0; j < subReply->elements; j+=2)
            {
                RedisReply* tmp = subReply->element[j];
                RedisReply* next = subReply->element[j+1];
                _CHECK_REPLY((&con), tmp, REDIS_REPLY_STRING, -1, false, 0, "[%s %s]", RedisCmd::HGETALL.c_str(), subQueueMetaName.c_str());
                LOG_DEBUG("queuename:[%s], subqueuemeta:[%s], client:[%s]", queue.option.msgQueueName.c_str(), subQueueMetaName.c_str(), tmp->str);
                queue.clients[j/2].client = tmp->str;
                queue.clients[j/2].lastVisit = atol(next->str);
            }
            //获取每个子队列的长度
            for (std::vector<Queue::SubQueue>::iterator itSubQueue = queue.clients.begin(); itSubQueue != queue.clients.end(); ++itSubQueue)
            {
                con.AppendCommand("%s %s", RedisCmd::LLEN.c_str(), QueueMetaData::GetQueueSubName(queue.option.msgQueueName, itSubQueue->client).c_str());
            }

            for (size_t i=0; i<queue.clients.size(); ++i)
            {
                RedisReply* getSubCountReply = NULL;
                std::string subQueueName = QueueMetaData::GetQueueSubName(queue.option.msgQueueName, queue.clients[i].client);
                if(REDIS_OK != con.GetReply(getSubCountReply))
                {
                    LOG_ERROR("get subqueue:[%s] count error, redis cmd:[%s %s]", subQueueName.c_str(), RedisCmd::LLEN.c_str(), subQueueName.c_str());
                    return -1;
                }
                BASE_BLOCK_GUARD(&RedisConnector::FreeReply, con, getSubCountReply);
                _CHECK_REPLY((&con), getSubCountReply, REDIS_REPLY_INTEGER, -1, true, queue.clients.size()-i-1, "[%s %s]", RedisCmd::LLEN.c_str(), subQueueName.c_str());
                queue.clients[i].msgCount = getSubCountReply->integer;
            }
        }
    }
    base::MutexGuard lock(&_rwMutex);
    assert(lock.Lock() == 0);
    _name2Queue.swap(name2QueueTmp);
    return 0;

}

int QueueMgr::PopMsg(const std::string& msgQueueCode, QueueMsg& msg)
{
    std::string queueName;
    std::string clientName;
    int msgType ; 
    int ret = 0; 
    {
        base::MutexGuard lock(&_rwMutex);
        assert(lock.Lock()==0);
        ret = GetQueueName(msgQueueCode, queueName, clientName);
        if(0 != ret)
        {
            return ret;
        }
        NAME2QUEUE::iterator itQueue = _name2Queue.find(queueName);
        assert(itQueue != _name2Queue.end());
        const QueueOption& option = itQueue->second.option;
        msgType = option.msgType;
    }
    RedisConnector* con  = GetConnector();
    assert(con);

    std::string queueMetaName ;
    std::string queueMsgName ;
    std::string timeProperty ;
    time_t now = time(NULL);
    RedisReply* reply = NULL;

    if (msgType == SUB2PUB)
    {
        queueMetaName = QueueMetaData::GetSubQueueMetaName(queueName);
        queueMsgName = QueueMetaData::GetQueueSubName(queueName, clientName);
        timeProperty = clientName;
    }
    else
    {
        queueMetaName = QueueMetaData::GetQueueMetaName(queueName);
        queueMsgName = QueueMetaData::GetQueueMsgName(queueName);
        timeProperty = "lastvisit";
    }

    con->AppendCommand("%s %s", RedisCmd::LPOP.c_str(), queueMsgName.c_str());
    con->AppendCommand("%s %s %s %d", RedisCmd::HSET.c_str(), queueMetaName.c_str(), timeProperty.c_str(), now);
    if (REDIS_OK != con->GetReply(reply))
    {
        LOG_ERROR("pop msgqueue[%s] error", queueName.c_str());
        return MsgQueueErrorNo::INTERNAL_ERROR;
    }
    BASE_BLOCK_GUARD(&RedisConnector::FreeReply, *con, reply);
    _CHECK_REPLY(con, reply, REDIS_REPLY_STRING, -1, true, 1, "[%s %s]", RedisCmd::LPOP.c_str(), queueMsgName.c_str());

    RedisReply* updateLastVisitReply = NULL;
    if (REDIS_OK != con->GetReply(updateLastVisitReply))
    {
        LOG_ERROR("pop msgqueue[%s] error", queueName.c_str());
    }
    BASE_BLOCK_GUARD(&RedisConnector::FreeReply, *con, updateLastVisitReply);
    _CHECK_REPLY(con, updateLastVisitReply, REDIS_REPLY_INTEGER, -1, true, 0, "[%s %s %s %ld]", RedisCmd::HSET.c_str(), queueMetaName.c_str(), timeProperty.c_str(), now);

    if (msg.Unserialize(reply->str, reply->len) < 0)
    {
        LOG_ERROR("unserialize msg error, data:[%s]", base::Escape(reply->str, reply->len).c_str());
        return MsgQueueErrorNo::INTERNAL_ERROR;
    }

    return 0;
}

int QueueMgr::PushMsg(const std::string& msgQueueCode, const QueueMsg& msg)
{
    std::string queueName;
    std::string clientName;
    int msgType;
    std::list<std::string>* listBuf = GetClientsBuf();
    assert(listBuf);
    int ret = 0;
    {
        base::MutexGuard lock(&_rwMutex);
        assert(lock.Lock()==0);
        ret = GetQueueName(msgQueueCode, queueName, clientName);
        if(0 != ret)
        {
            return ret;
        }
        NAME2QUEUE::const_iterator it = _name2Queue.find(queueName);
        assert(it != _name2Queue.end());

        if ( it->second.option.maxCount > 0 && (int)it->second.msgCount >= it->second.option.maxCount)
        {
            LOG_DEBUG("MSG_LIMIT, queuename:[%s], queue:[%s]", queueName.c_str(), it->second.ToString().c_str());
            return MsgQueueErrorNo::MSG_LIMIT;
        }
        const Queue& queue = it->second;
        msgType = queue.option.msgType;
        listBuf->clear();
        for (std::vector<Queue::SubQueue>::const_iterator itClient = queue.clients.begin(); itClient!= queue.clients.end(); ++itClient)
        {
            listBuf->push_back(itClient->client);
        }
        LOG_DEBUG("queueinfo:%s", queue.ToString().c_str());
    }

    if (msgType == SUB2PUB && listBuf->size() == 0)
    {
        LOG_INFO("client:[%s] push msg to queue:[%s], no subsriber", clientName.c_str(), queueName.c_str());
        return MsgQueueErrorNo::SUCCESS;
    }

    RedisConnector* con  = GetConnector();
    assert(con);

    std::string data = msg.Serialize();
    time_t now = time(NULL);

    if (msgType == SUB2PUB)
    {
        for (std::list<std::string>::const_iterator it = listBuf->begin(); it != listBuf->end(); ++it)
        {
            con->AppendCommand("%s %s %b", RedisCmd::RPUSH.c_str(), QueueMetaData::GetQueueSubName(queueName, *it).c_str(), data.c_str(), data.size());
            con->AppendCommand("%s %s %s %d", RedisCmd::HSET.c_str(), QueueMetaData::GetSubQueueMetaName(queueName).c_str(), it->c_str(), now);
        }
    }
    else 
    {
        con->AppendCommand("%s %s %b", RedisCmd::RPUSH.c_str(), QueueMetaData::GetQueueMsgName(queueName).c_str(), data.c_str(), data.size());
        con->AppendCommand("%s %s %s %d", RedisCmd::HSET.c_str(), QueueMetaData::GetQueueMetaName(queueName).c_str(), "lastvisit", now);
    }

    size_t i = 0;
    size_t totalReply = (msgType == SUB2PUB ? listBuf->size()*2 : 2);
    do 
    {
        LOG_DEBUG("push msg, time:%ld, msgType:%d", now, msgType);
        RedisReply* pushReply=NULL;
        if(REDIS_OK != con->GetReply(pushReply))
        {
            LOG_ERROR("push msgqueue[%s] error, msg:[%s]", queueName.c_str(), msg.data.c_str());
            return MsgQueueErrorNo::INTERNAL_ERROR;
        }
        BASE_BLOCK_GUARD(&RedisConnector::FreeReply, *con, pushReply);
        _CHECK_REPLY(con, pushReply, REDIS_REPLY_INTEGER, -1, true, --totalReply, "%s", "pushmsg");

        RedisReply* updateVisitRelpy=NULL;
        if(REDIS_OK != con->GetReply(updateVisitRelpy))
        {
            LOG_ERROR("push msgqueue[%s] error, msg:[%s]", queueName.c_str(), msg.data.c_str());
            return MsgQueueErrorNo::INTERNAL_ERROR;
        }
        BASE_BLOCK_GUARD(&RedisConnector::FreeReply, *con, updateVisitRelpy);
        _CHECK_REPLY(con, updateVisitRelpy, REDIS_REPLY_INTEGER, -1, true, --totalReply, "%s", "updatelastvisit");
        LOG_DEBUG("push msg succ");
        ++i;
    } while (i<listBuf->size() && msgType == SUB2PUB);
    return 0;
}

RedisConnector* QueueMgr::GetConnector()
{
    RedisConnector* con = static_cast<RedisConnector*>(_redisConn.Get());
    if (!con)
    {
        con = new RedisConnector;
        con->Open(_redisAddr, _msTimeout, _pwd);
        _redisConn.Set(con);
    }
    return con;
}

void QueueMgr::ReleaseConnector(void* con)
{
    RedisConnector * redisCon = static_cast<RedisConnector*>(con);
    if (redisCon)
    {
        redisCon->Close();
        delete redisCon;
    }
}

std::string QueueMgr::GenMsgQueueCode()
{
    uuid_t uuid;
    uuid_generate(uuid);
    char buf[2*sizeof(uuid_t)] = {0};
    for (size_t i=0; i<sizeof(uuid_t)/sizeof(char); ++i)
    {
        sprintf(buf+2*i, "%02x", uuid[i]);
    }
    return buf;
}

void QueueMgr::RemoveExpireMsgQueueCode(time_t expireTime)
{
    base::MutexGuard lock(&_rwMutex);
    assert(lock.Lock() == 0);
    MSGCODESEQUENCE::reverse_iterator it;
    int i =0; 
    for (it = _msgCodeSequence.rbegin(); it != _msgCodeSequence.rend() && i<_removeOnceCount; ++it,++i)
    {
        if (it->time >= expireTime)
        {
            break;
        }
        QUEUECODE2QUEUENAME::iterator itQueueCodeQueueName = _msgQueueCode2QueueName.find(it->msgQueueCode);
        if (itQueueCodeQueueName == _msgQueueCode2QueueName.end())
        {
            LOG_ERROR("remove expire msgcode, msgCode:[%s], queuename is no exist", it->msgQueueCode.c_str());
        }
        LOG_DEBUG("remove msgQueueCode:[%s], clientName:[%s], queueName:[%s], time:[%d], expire:[%ld]", 
            it->msgQueueCode.c_str(), 
            itQueueCodeQueueName->second.clientName.c_str(), 
            itQueueCodeQueueName->second.queueName.c_str(), it->time, expireTime);
        _client2MsgCode.erase(std::make_pair(itQueueCodeQueueName->second.clientName, itQueueCodeQueueName->second.queueName));
        _msgQueueCode2QueueName.erase(itQueueCodeQueueName);
    }
    if (it != _msgCodeSequence.rbegin())
    {
        for (MSGCODESEQUENCE::iterator itSeq = _msgCodeSequence.begin(); itSeq != _msgCodeSequence.end(); ++itSeq)
        {
            LOG_DEBUG("remove sequence:->[%s][%d]", itSeq->msgQueueCode.c_str(), itSeq->time);
        }
        _msgCodeSequence.erase(it.base(), _msgCodeSequence.end());
    }
}

void QueueMgr::RemoveExpireQueue(time_t /*expireTime*/)
{
    base::MutexGuard lock(&_rwMutex);
    lock.Lock();
    for (NAME2QUEUE::iterator it = _name2Queue.begin(); it != _name2Queue.end(); ++it)
    {

    }
}

void QueueMgr::GetClients(const std::vector<std::string> msgCodes, std::vector<QueueClient>& clientInfos) const
{
    base::MutexGuard lock(&_rwMutex);
    assert(lock.Lock() == 0);

    for (std::vector<std::string>::const_iterator it = msgCodes.begin(); it != msgCodes.end(); ++it)
    {
        QUEUECODE2QUEUENAME::const_iterator itQueue = _msgQueueCode2QueueName.find(*it);
        if (itQueue == _msgQueueCode2QueueName.end())
        {
            clientInfos.push_back(QueueClient());
            clientInfos.back().msgQueueCode = *it;
        }
        else
        {
            clientInfos.push_back(QueueClient());
            clientInfos.back().msgQueueCode = *it;
            clientInfos.back().clientName = itQueue->second.clientName;
            clientInfos.back().queueName = itQueue->second.queueName;
        }
    }
}

void QueueMgr::Ping()
{
    RedisConnector* con = GetConnector();
    con->Command("%s", RedisCmd::PING.c_str());
    LOG_DEBUG("ping redis:[%s]", _redisAddr.ToString().c_str());
}

int QueueMgr::GetAllQueue(std::vector<Queue>& queues)
{
    base::MutexGuard lock(&_rwMutex);
    assert(lock.Lock() == 0);
    for (NAME2QUEUE::const_iterator it = _name2Queue.begin(); it != _name2Queue.end(); ++it)
    {
        queues.push_back(it->second);
    }
    return 0;
}

#undef  _CHECK_REPLY

#undef  _CHECK_REPLY
