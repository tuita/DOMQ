#ifndef _QUEUE_H__
#define _QUEUE_H__

#include <string>
#include <vector>
#include <base/net/sockaddr.h>

struct QueueOption
{
    std::string msgQueueName;
    int msgType;
    int duration;//µ•Œª∫¡√Î
    int maxCount;
};

struct Queue
{
    struct SubQueue
    {
        std::string client;
        size_t msgCount;
        time_t lastVisit;
    };
    QueueOption option;
    base::SockAddress redisAddr;
    size_t msgCount;
    time_t lastVisit;

    std::vector<SubQueue> clients;

    std::string ToString() const;
};

struct QueueClient
{
    std::string msgQueueCode;
    std::string clientName;
    std::string queueName;

    std::string ToString() const;
};

#endif

