#ifndef _QUEUE_MSG_FACTORY_H__
#define _QUEUE_MSG_FACTORY_H__

#include <list>
#include <netmgr/factory.h>

class QueueMsgFactory : public netmgr::MsgFactory
{
public:
    virtual std::list<std::string> KnownTypes() const;
    virtual netmgr::Message* Create(const char* type);
    virtual void Destroy(const netmgr::Message* msg);
};
#endif
