#include <list>
#include <string.h>
#include <netmgr/message.h>
#include "msgfactory.h"

namespace netmgr
{
std::list<std::string> NetMgrMsgFactory::KnownTypes() const
{
    std::list<std::string> result;
    result.push_back("netmgr::ServiceMessage");
    result.push_back("netmgr::LineMessage");
    result.push_back("netmgr::IndexMsg");
    return result;
    
}

#define NAME2OBJ(x) \
    if (strcmp(type, #x) == 0) { \
    return new x; \
    }
Message* NetMgrMsgFactory::Create(const char* type)
{
    NAME2OBJ(netmgr::ServiceMessage)
    NAME2OBJ(netmgr::LineMessage)

    return NULL;
}
#undef NAME2OBJ

void NetMgrMsgFactory::Destroy(const Message* msg)
{
    delete msg;
}
}

