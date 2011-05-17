#ifndef MSGFACTORY_H__
#define MSGFACTORY_H__

#include <list>
#include <netmgr/factory.h>

namespace netmgr
{
    class Message;
}

class MsgFactory:public netmgr::MsgFactory
{
public:
    virtual std::list<std::string> KnownTypes() const ;
    virtual netmgr::Message* Create(const char* type) ;
    virtual void Destroy(const netmgr::Message* msg) ;
};

#endif

