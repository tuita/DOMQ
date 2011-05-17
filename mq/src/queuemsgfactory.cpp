#include "queuemsgfactory.h"
#include <list>
#include <string.h>
#include <netmgr/message.h>
#include <msg/msgqueuemsg.h>

std::list<std::string> QueueMsgFactory::KnownTypes() const
{
    std::list<std::string> result;
    result.push_back("OpenMsgQueueRequest");
    result.push_back("OpenMsgQueueResult");
    result.push_back("CloseMsgQueueRequest");
    result.push_back("CloseMsgQueueRequest");
    result.push_back("GetMsgRequest");
    result.push_back("GetMsgResult");
    result.push_back("SendMsgRequest");
    result.push_back("SendMsgResult");
    return result;

}

#define NAME2OBJ(x) \
    if (strcmp(type, #x) == 0) { \
    return new x; \
    }
netmgr::Message* QueueMsgFactory::Create(const char* type)
{
    NAME2OBJ(OpenMsgQueueRequest)
    NAME2OBJ(OpenMsgQueueResult)
    NAME2OBJ(CloseMsgQueueRequest)
    NAME2OBJ(CloseMsgQueueResult)
    NAME2OBJ(GetMsgRequest)
    NAME2OBJ(GetMsgResult)
    NAME2OBJ(SendMsgRequest)
    NAME2OBJ(SendMsgResult)
    return NULL;
}
#undef NAME2OBJ

void QueueMsgFactory::Destroy(const netmgr::Message* msg)
{
    delete msg;
}
