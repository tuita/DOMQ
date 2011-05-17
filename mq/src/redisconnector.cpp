#include "redisconnector.h"
#include <base/log/log.h>
#include <base/net/sockaddr.h>
#include <msg/msgqueueerrno.h>
#include <base/util/util.h>
#include <cerrno>
#include <cassert>
#include <sys/time.h>

const  std::string RedisCmd::APPEND="APPEND";
const  std::string RedisCmd::AUTH="AUTH";
const  std::string RedisCmd::BGREWRITEAOF="BGREWRITEAOF";
const  std::string RedisCmd::BGSAVE="BGSAVE";
const  std::string RedisCmd::BLPOP="BLPOP";
const  std::string RedisCmd::BRPOP="BRPOP";
const  std::string RedisCmd::BRPOPLPUSH="BRPOPLPUSH";
const  std::string RedisCmd::DBSIZE="DBSIZE";
const  std::string RedisCmd::DECR="DECR";
const  std::string RedisCmd::DECRBY="DECRBY";
const  std::string RedisCmd::DEL="DEL";
const  std::string RedisCmd::DISCARD="DISCARD";
const  std::string RedisCmd::ECHO="ECHO";
const  std::string RedisCmd::EXEC="EXEC";
const  std::string RedisCmd::EXISTS="EXISTS";
const  std::string RedisCmd::EXPIRE="EXPIRE";
const  std::string RedisCmd::EXPIREAT="EXPIREAT";
const  std::string RedisCmd::FLUSHALL="FLUSHALL";
const  std::string RedisCmd::FLUSHDB="FLUSHDB";
const  std::string RedisCmd::GET="GET";
const  std::string RedisCmd::GETBIT="GETBIT";
const  std::string RedisCmd::GETRANGE="GETRANGE";
const  std::string RedisCmd::GETSET="GETSET";
const  std::string RedisCmd::HDEL="HDEL";
const  std::string RedisCmd::HEXISTS="HEXISTS";
const  std::string RedisCmd::HGET="HGET";
const  std::string RedisCmd::HGETALL="HGETALL";
const  std::string RedisCmd::HINCRBY="HINCRBY";
const  std::string RedisCmd::HKEYS="HKEYS";
const  std::string RedisCmd::HLEN="HLEN";
const  std::string RedisCmd::HMGET="HMGET";
const  std::string RedisCmd::HMSET="HMSET";
const  std::string RedisCmd::HSET="HSET";
const  std::string RedisCmd::HSETNX="HSETNX";
const  std::string RedisCmd::HVALS="HVALS";
const  std::string RedisCmd::INCR="INCR";
const  std::string RedisCmd::INCRBY="INCRBY";
const  std::string RedisCmd::INFO="INFO";
const  std::string RedisCmd::KEYS="KEYS";
const  std::string RedisCmd::LASTSAVE="LASTSAVE";
const  std::string RedisCmd::LINDEX="LINDEX";
const  std::string RedisCmd::LINSERT="LINSERT";
const  std::string RedisCmd::LLEN="LLEN";
const  std::string RedisCmd::LPOP="LPOP";
const  std::string RedisCmd::LPUSH="LPUSH";
const  std::string RedisCmd::LPUSHX="LPUSHX";
const  std::string RedisCmd::LRANGE="LRANGE";
const  std::string RedisCmd::LREM="LREM";
const  std::string RedisCmd::LSET="LSET";
const  std::string RedisCmd::LTRIM="LTRIM";
const  std::string RedisCmd::MGET="MGET";
const  std::string RedisCmd::MONITOR="MONITOR";
const  std::string RedisCmd::MOVE="MOVE";
const  std::string RedisCmd::MSET="MSET";
const  std::string RedisCmd::MSETNX="MSETNX";
const  std::string RedisCmd::MULTI="MULTI";
const  std::string RedisCmd::PERSIST="PERSIST";
const  std::string RedisCmd::PING="PING";
const  std::string RedisCmd::PSUBSCRIBE="PSUBSCRIBE";
const  std::string RedisCmd::PUBLISH="PUBLISH";
const  std::string RedisCmd::PUNSUBSCRIBE="PUNSUBSCRIBE";
const  std::string RedisCmd::QUIT="QUIT";
const  std::string RedisCmd::RANDOMKEY="RANDOMKEY";
const  std::string RedisCmd::RENAME="RENAME";
const  std::string RedisCmd::RENAMENX="RENAMENX";
const  std::string RedisCmd::RPOP="RPOP";
const  std::string RedisCmd::RPOPLPUSH="RPOPLPUSH";
const  std::string RedisCmd::RPUSH="RPUSH";
const  std::string RedisCmd::RPUSHX="RPUSHX";
const  std::string RedisCmd::SADD="SADD";
const  std::string RedisCmd::SAVE="SAVE";
const  std::string RedisCmd::SCARD="SCARD";
const  std::string RedisCmd::SDIFF="SDIFF";
const  std::string RedisCmd::SDIFFSTORE="SDIFFSTORE";
const  std::string RedisCmd::SELECT="SELECT";
const  std::string RedisCmd::SET="SET";
const  std::string RedisCmd::SETBIT="SETBIT";
const  std::string RedisCmd::SETEX="SETEX";
const  std::string RedisCmd::SETNX="SETNX";
const  std::string RedisCmd::SETRANGE="SETRANGE";
const  std::string RedisCmd::SHUTDOWN="SHUTDOWN";
const  std::string RedisCmd::SINTER="SINTER";
const  std::string RedisCmd::SINTERSTORE="SINTERSTORE";
const  std::string RedisCmd::SISMEMBER="SISMEMBER";
const  std::string RedisCmd::SLAVEOF="SLAVEOF";
const  std::string RedisCmd::SMEMBERS="SMEMBERS";
const  std::string RedisCmd::SMOVE="SMOVE";
const  std::string RedisCmd::SORT="SORT";
const  std::string RedisCmd::SPOP="SPOP";
const  std::string RedisCmd::SRANDMEMBER="SRANDMEMBER";
const  std::string RedisCmd::SREM="SREM";
const  std::string RedisCmd::STRLEN="STRLEN";
const  std::string RedisCmd::SUBSCRIBE="SUBSCRIBE";
const  std::string RedisCmd::SUNION="SUNION";
const  std::string RedisCmd::SUNIONSTORE="SUNIONSTORE";
const  std::string RedisCmd::SYNC="SYNC";
const  std::string RedisCmd::TTL="TTL";
const  std::string RedisCmd::TYPE="TYPE";
const  std::string RedisCmd::UNSUBSCRIBE="UNSUBSCRIBE";
const  std::string RedisCmd::UNWATCH="UNWATCH";
const  std::string RedisCmd::WATCH="WATCH";
const  std::string RedisCmd::ZADD="ZADD";
const  std::string RedisCmd::ZCARD="ZCARD";
const  std::string RedisCmd::ZCOUNT="ZCOUNT";
const  std::string RedisCmd::ZINCRBY="ZINCRBY";
const  std::string RedisCmd::ZINTERSTORE="ZINTERSTORE";
const  std::string RedisCmd::ZRANGE="ZRANGE";
const  std::string RedisCmd::ZRANGEBYSCORE="ZRANGEBYSCORE";
const  std::string RedisCmd::ZRANK="ZRANK";
const  std::string RedisCmd::ZREM="ZREM";
const  std::string RedisCmd::ZREMRANGEBYRANK="ZREMRANGEBYRANK";
const  std::string RedisCmd::ZREMRANGEBYSCORE="ZREMRANGEBYSCORE";
const  std::string RedisCmd::ZREVRANGE="ZREVRANGE";
const  std::string RedisCmd::ZREVRANGEBYSCORE="ZREVRANGEBYSCORE";
const  std::string RedisCmd::ZREVRANK="ZREVRANK";
const  std::string RedisCmd::ZSCORE="ZSCORE";
const  std::string RedisCmd::ZUNIONSTORE="ZUNIONSTORE";

int RedisConnector::Open(base::SockAddress& addr, int msTimeout, const std::string& pwd)
{
    _addr = addr ; 
    _msTimeout = msTimeout;
    _pwd = pwd;
    _connected = false;
    return Connect();
}

int RedisConnector::GetLastErrNo()
{
    return _redis? _redis->err: 0;
}

const char* RedisConnector::GetLastError()
{
    return _redis ? _redis->errstr : "";
}
int RedisConnector::Connect()
{
    struct timeval tv ={_msTimeout/1000, _msTimeout%1000*1000};
    
    LOG_DEBUG("redis connect:[%s] start", _addr.ToString().c_str());
    _redis = redisConnectWithTimeout(_addr.GetHost().c_str(), _addr.GetPort(), tv);
    if (0 != _redis->err)
    {
        LOG_ERROR("connect redis:[%s] error, errno:[%d], errmsg:[%s]", _addr.ToString().c_str(), _redis->err, _redis->errstr);
        return -1;
    }
    LOG_DEBUG("redis connect:[%s] succ", _addr.ToString().c_str());
    assert(_redis);
    /*int ret = credis_auth(_redis, _pwd.c_str());
    if (ret != CREDIS_OK)
    {
        LOG_ERROR("connect redis:[%s], passwd:[%s] error:[%s]", _addr.ToString().c_str(), _pwd.c_str(), MsgQueueError(ret));
        return ret;
    }*/
    _connected = true;
    return 0;
}

void RedisConnector::Close()
{
    _connected = false;
    if (_redis)
    {
        _connected = false;
        redisFree(_redis);
        _redis = NULL;
    }
}

RedisReply* RedisConnector::Command(const char* format, ...)
{
    timeval tv1;
    gettimeofday(&tv1, NULL);
    if (!_connected && Connect() != 0)
    {
        return NULL;
    }
    va_list ap;
    RedisReply *reply = NULL;
    va_start(ap,format);
    reply = static_cast<RedisReply*>(redisvCommand(_redis,format,ap));
    va_end(ap);
    if (reply == NULL)
    {
        LOG_ERROR("getreply error, errno:[%d],error:[%s], rediserrno:[%d], errmsg:[%s]", errno, strerror(errno), _redis->err, _redis->errstr);
        Close();
    }

    timeval tv2;
    gettimeofday(&tv2, NULL);
    int interval = base::time_interval(tv2, tv1);
    if (interval > 0)
    {
        LOG_DEBUG("redis exec time:[%d] us" ,interval);
    }
    
    return reply;
}

void RedisConnector::AppendCommand(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    assert(_redis);
    redisvAppendCommand(_redis, format, ap);
    va_end(ap);
}

int RedisConnector::GetReply(RedisReply*& reply)
{
    timeval tv1;
    gettimeofday(&tv1, NULL);
   
    int ret = 0 ; 
    const int RETRY = 2;
    for (int i=0; i<RETRY; ++i)
    {
        if (!_connected && Connect() != 0)
        {
            ret = REDIS_ERR;
            continue;
        }
        assert(_redis);
        ret = redisGetReply(_redis, reinterpret_cast<void**>(&reply));
        if(REDIS_OK != ret)
        {
            assert(_redis&&_redis->errstr);
            LOG_ERROR("getreply error, errno:[%d],error:[%s], rediserrno:[%d], errmsg:[%s]", errno, strerror(errno), _redis->err, _redis->errstr);
            Close();
        }
        else
        {
            timeval tv2;
            gettimeofday(&tv2, NULL);
            int interval = base::time_interval(tv2, tv1);
            if (interval > 0)
            {
                LOG_DEBUG("redis exec time:[%d] us" ,interval);
            }
            break;
        }
        
    }
    return ret;
}

void RedisConnector::FreeReply(void* reply)
{
    if (reply)
    {
        freeReplyObject(reply);
    }
}


