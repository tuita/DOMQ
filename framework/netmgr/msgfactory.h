#ifndef _NETMGR_MSG_FACTORY_H_
#define _NETMGR_MSG_FACTORY_H_

#include <list>
#include <netmgr/factory.h>

namespace netmgr
{
class Message;
    
class NetMgrMsgFactory:public MsgFactory
{
public:
    NetMgrMsgFactory() {}
    virtual ~NetMgrMsgFactory() {}
    virtual std::list<std::string> KnownTypes() const ;
    virtual Message* Create(const char* type) ;
    virtual void Destroy(const Message* msg) ;
};

}
#endif

