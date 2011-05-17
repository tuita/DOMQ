#include "networkoption.h"
#include <base/log/log.h>
namespace netmgr {

int ParseProtocol(const char* str)
{
#define __STRING2PROTOCOL__(_PROTOCOLBUF_)  if (strcmp(str, #_PROTOCOLBUF_) == 0) return netmgr::_PROTOCOLBUF_
	__STRING2PROTOCOL__(RAWDATA_PROTOCOL);
    __STRING2PROTOCOL__(JSON_PROTOCOL);
#undef __STRING2PROTOCOL__
    LOG_ERROR("Unknown protocol: %s", str);
    return -1;
}

}