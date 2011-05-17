#ifndef _REDIS_CONNECTOR_H__
#define _REDIS_CONNECTOR_H__

#include <base/net/sockaddr.h>
#include <string>
#include <vector>
#include <hiredis/hiredis.h>

namespace ac
{
    class SockAddr;
}
struct RedisCmd
{
    const static std::string APPEND;
    const static std::string AUTH;
    const static std::string BGREWRITEAOF;
    const static std::string BGSAVE;
    const static std::string BLPOP;
    const static std::string BRPOP;
    const static std::string BRPOPLPUSH;
    const static std::string DBSIZE;
    const static std::string DECR;
    const static std::string DECRBY;
    const static std::string DEL;
    const static std::string DISCARD;
    const static std::string ECHO;
    const static std::string EXEC;
    const static std::string EXISTS;
    const static std::string EXPIRE;
    const static std::string EXPIREAT;
    const static std::string FLUSHALL;
    const static std::string FLUSHDB;
    const static std::string GET;
    const static std::string GETBIT;
    const static std::string GETRANGE;
    const static std::string GETSET;
    const static std::string HDEL;
    const static std::string HEXISTS;
    const static std::string HGET;
    const static std::string HGETALL;
    const static std::string HINCRBY;
    const static std::string HKEYS;
    const static std::string HLEN;
    const static std::string HMGET;
    const static std::string HMSET;
    const static std::string HSET;
    const static std::string HSETNX;
    const static std::string HVALS;
    const static std::string INCR;
    const static std::string INCRBY;
    const static std::string INFO;
    const static std::string KEYS;
    const static std::string LASTSAVE;
    const static std::string LINDEX;
    const static std::string LINSERT;
    const static std::string LLEN;
    const static std::string LPOP;
    const static std::string LPUSH;
    const static std::string LPUSHX;
    const static std::string LRANGE;
    const static std::string LREM;
    const static std::string LSET;
    const static std::string LTRIM;
    const static std::string MGET;
    const static std::string MONITOR;
    const static std::string MOVE;
    const static std::string MSET;
    const static std::string MSETNX;
    const static std::string MULTI;
    const static std::string PERSIST;
    const static std::string PING;
    const static std::string PSUBSCRIBE;
    const static std::string PUBLISH;
    const static std::string PUNSUBSCRIBE;
    const static std::string QUIT;
    const static std::string RANDOMKEY;
    const static std::string RENAME;
    const static std::string RENAMENX;
    const static std::string RPOP;
    const static std::string RPOPLPUSH;
    const static std::string RPUSH;
    const static std::string RPUSHX;
    const static std::string SADD;
    const static std::string SAVE;
    const static std::string SCARD;
    const static std::string SDIFF;
    const static std::string SDIFFSTORE;
    const static std::string SELECT;
    const static std::string SET;
    const static std::string SETBIT;
    const static std::string SETEX;
    const static std::string SETNX;
    const static std::string SETRANGE;
    const static std::string SHUTDOWN;
    const static std::string SINTER;
    const static std::string SINTERSTORE;
    const static std::string SISMEMBER;
    const static std::string SLAVEOF;
    const static std::string SMEMBERS;
    const static std::string SMOVE;
    const static std::string SORT;
    const static std::string SPOP;
    const static std::string SRANDMEMBER;
    const static std::string SREM;
    const static std::string STRLEN;
    const static std::string SUBSCRIBE;
    const static std::string SUNION;
    const static std::string SUNIONSTORE;
    const static std::string SYNC;
    const static std::string TTL;
    const static std::string TYPE;
    const static std::string UNSUBSCRIBE;
    const static std::string UNWATCH;
    const static std::string WATCH;
    const static std::string ZADD;
    const static std::string ZCARD;
    const static std::string ZCOUNT;
    const static std::string ZINCRBY;
    const static std::string ZINTERSTORE;
    const static std::string ZRANGE;
    const static std::string ZRANGEBYSCORE;
    const static std::string ZRANK;
    const static std::string ZREM;
    const static std::string ZREMRANGEBYRANK;
    const static std::string ZREMRANGEBYSCORE;
    const static std::string ZREVRANGE;
    const static std::string ZREVRANGEBYSCORE;
    const static std::string ZREVRANK;
    const static std::string ZSCORE;
    const static std::string ZUNIONSTORE;
};

typedef redisReply RedisReply;

class RedisConnector
{
public:
    int Open(base::SockAddress& addr, int msTimeout, const std::string& pwd);
    void Close();

    void AppendCommand(const char *format, ...);
    int  GetReply(RedisReply *& reply);
    RedisReply *Command(const char *format, ...);
    void FreeReply(void* reply);

    int Connect();

    int GetLastErrNo();
    const char* GetLastError(); 

private:
    base::SockAddress _addr;
    int _msTimeout;
    std::string _pwd;
    redisContext*  _redis;
    bool _connected;
};

#endif
