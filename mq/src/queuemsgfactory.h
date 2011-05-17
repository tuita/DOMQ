#ifndef _QUEUE_MSG_FACTORY_H__
#define _QUEUE_MSG_FACTORY_H__

#include <list>
#include <netmgr/factory.h>
#include <base/util/allocator.h>
#include <msg/msgqueuemsg.h>
#include <base/thread/mutex.h>

#define  _MSGTYPES_(v) \
        v(OpenMsgQueueRequest)\
        v(OpenMsgQueueResult)\
        v(CloseMsgQueueRequest)\
        v(CloseMsgQueueResult)\
        v(GetMsgRequest)\
        v(GetMsgResult)\
        v(SendMsgRequest)\
        v(SendMsgResult)

#define  DECLARE_ALLOCATOR(T) base::ObjectPoolAllocator<T> _#T

class QueueMsgFactory : public netmgr::MsgFactory
{
public:
    QueueMsgFactory();
    virtual std::list<std::string> KnownTypes() const;
    virtual netmgr::Message* Create(const char* type);
    virtual void Destroy(const netmgr::Message* msg);


private:
    
#define  DECLARE_MSG_ALLOCATOR(MSGNAME) base::ObjectPoolCallBackAllocator<MSGNAME> _##MSGNAME;base::ThreadMutex _mutex##MSGNAME;
         _MSGTYPES_(DECLARE_MSG_ALLOCATOR)
#undef  DECLARE_MSG_ALLOCATOR
        bool _debug;
};
#endif
