#ifndef _QUEUEMSG_H__
#define _QUEUEMSG_H__

#include <string>

class QueueMsg
{
public:
    std::string data;

    std::string Serialize() const;
    int Serialize(char* buf, size_t bufLen) const;

    int Unserialize(const std::string&);
    int Unserialize(const char* buf, size_t bufLen);

};
#endif

