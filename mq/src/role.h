#ifndef _ROLE_H__
#define _ROLE_H__
#include <base/net/sockaddr.h>

enum QueueRole
{
    SUBSCRIBER = 0, 
    PUBLISHER = 1,
};

class Subscriber
{
private:
    base::SockAddress addr;
    std::string name;
};

class  Publisher
{
private:
    base::SockAddress addr;
    std::string name;
};

#endif

