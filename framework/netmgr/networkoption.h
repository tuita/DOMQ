#ifndef _NETMGR_NETWORKOPTION_H__
#define _NETMGR_NETWORKOPTION_H__

#include <base/net/sockaddr.h>
#include <netmgr/coder.h>

namespace netmgr {

enum  ProtocolType
{
    RAWDATA_PROTOCOL = 0x00,
    RAWHEADBODY_PROTOCOL = 0x01,
    RAW_JSONSTS_PROTOCOL = 0x20,
    RAW_PROTOCOLBUF_PROTOCOL = 0x40,
    RAW_JSON_PROTOCOL = 0x80,
    SERVICE_FLAG = 0x1000000,
    PROTOCOLBUF_PROTOCOL = RAWHEADBODY_PROTOCOL|RAW_PROTOCOLBUF_PROTOCOL,
    JSON_PROTOCOL = RAWHEADBODY_PROTOCOL | RAW_JSON_PROTOCOL,
    STAND_PROTOCOL = PROTOCOLBUF_PROTOCOL | SERVICE_FLAG,
    JSONSTS_PROTOCOL = RAWHEADBODY_PROTOCOL|RAW_JSONSTS_PROTOCOL| SERVICE_FLAG,
};
int ParseProtocol(const char* str);

struct NetworkOption
{
    base::SockAddress address;
    base::SockAddress localAddress;
    int protocol;  // connection protocol
    int firstTimeout; // first connection timeout, default is -1 (alive forver)
    int timeout;   // connection timeout, default is -1 (alive forver)
    int reconnect; // reconnect interval, default is 3 seconds

    size_t sendBufSize;
    size_t recvBufSize;

    ICoder* coder;

    NetworkOption():protocol(JSON_PROTOCOL), firstTimeout(-1), timeout(-1),
        reconnect(3000), sendBufSize(0), recvBufSize(0), coder(NULL)
    {
    }
};

}

#endif

