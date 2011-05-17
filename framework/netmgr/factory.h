#ifndef _NETMGR_FACTORY_H_
#define _NETMGR_FACTORY_H_

#include <string>
#include <list>
#include <map>
#include <base/thread/mutex.h>

namespace netmgr {
class Message;

struct MsgFactory
{
    MsgFactory() {}
    virtual ~MsgFactory() {}
    virtual std::list<std::string> KnownTypes() const = 0;
    virtual Message* Create(const char* type) = 0;
    virtual void Destroy(const Message* msg) = 0;

private:
    MsgFactory(const MsgFactory&);
    MsgFactory& operator=(const MsgFactory&);
};

class MappedMsgFactory : public MsgFactory
{
private:
    std::map<std::string, MsgFactory*> factories;

public:
    int RegisterFactory(MsgFactory* factory);
    void UnregisterFactory(MsgFactory* factory);

    virtual std::list<std::string> KnownTypes() const;
    virtual Message* Create(const char* type);
    virtual void Destroy(const Message* msg);

    base::ThreadMutex _mutex;
};

MappedMsgFactory* GetGlobalMsgFactory();

} 

#endif 

