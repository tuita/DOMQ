#ifndef _NETMGR_HANDLER_REGISTRY_H_
#define _NETMGR_HANDLER_REGISTRY_H_

#include <base/net/eventhandler.h>
#include <base/thread/mutex.h>
#include <ctype.h>
#include <map>
#include <string>

namespace netmgr
{

class EventHandlerRegistry
{
protected:
    std::map<std::string, base::EventHandler*> _name2Handlers;

public:
    EventHandlerRegistry()
    { }
    base::EventHandler* GetHandler(const std::string& name) const;
    int RegisterHandler(const std::string& name, base::EventHandler* handler);
    void RemoveHandler(const std::string& name);

    size_t GetCount() const;
    base::ThreadRWMutex _mutex;
};

} 

#endif

