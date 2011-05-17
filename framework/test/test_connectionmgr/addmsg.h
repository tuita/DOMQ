#ifndef __ADD_MSG_H__
#define __ADD_MSG_H__
#include <netmgr/message.h>
#include <stdint.h>

class AddRequest : public netmgr::Request
{
public:
    uint32_t param1;
    uint32_t param2;

    int ProtocolBufEncode(char* buf, size_t len) const;
    int ProtocolBufDecode(const char* buf, size_t len);
};

class AddResult : public netmgr::Result
{
public:
    uint32_t sum;
    int ProtocolBufEncode(char* buf, size_t len) const;
    int ProtocolBufDecode(const char* buf, size_t len);
};

#endif
