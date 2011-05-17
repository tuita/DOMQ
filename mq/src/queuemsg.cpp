#include "queuemsg.h"
#include <string.h>

int QueueMsg::Serialize(char* buf, size_t bufLen) const 
{
    if (data.size() > bufLen)
    {
        return -1;
    }
    memcpy(buf,  data.c_str(), data.size());
    return (int)data.size();
}

std::string QueueMsg::Serialize() const 
{
    return data;
}

int QueueMsg::Unserialize(const char* buf, size_t bufLen)
{
    data.assign(buf, bufLen);
    return bufLen;
}

int QueueMsg::Unserialize(const std::string& buf)
{
    data = buf;
    return data.size();
}

