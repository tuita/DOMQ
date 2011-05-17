#include "queuemsgfactory.h"
#include <list>
#include <string.h>
#include <netmgr/message.h>
#include <msg/msgqueuemsg.h>
//ObjectPoolAllocator(size_t iInitNum, size_t iMaxNum=0, size_t iIncNum=120, IMutex * pMutex=NilMutex::Instance())
#define  _INIT_ALLOCATE(CLASSNAME)  _##CLASSNAME(100, 0, 100, &(_mutex##CLASSNAME)),
QueueMsgFactory::QueueMsgFactory()
:_MSGTYPES_(_INIT_ALLOCATE)_debug(true)
{

}
#undef _INIT_ALLOCATE

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

#ifdef USEOBJECTPOOL
#define NAME2OBJ(CLASSNAME) \
    if (strcmp(type, #CLASSNAME) == 0) { \
     return _##CLASSNAME.Create();\
    }
#else 
#define NAME2OBJ(CLASSNAME) \
    if (strcmp(type, #CLASSNAME) == 0) { \
    return new CLASSNAME;\
    }
#endif

netmgr::Message* QueueMsgFactory::Create(const char* type)
{
    _MSGTYPES_(NAME2OBJ)
    return NULL;
}
#undef NAME2OBJ

void QueueMsgFactory::Destroy(const netmgr::Message* msg)
{
    ((netmgr::Message*)msg)->Dispose();
}
