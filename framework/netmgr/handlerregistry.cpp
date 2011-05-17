#include <netmgr/handlerregistry.h>
#include <base/thread/mutexguard.h>
using namespace base;

namespace netmgr
{

EventHandler* EventHandlerRegistry::GetHandler(const std::string& name) const
{
    
    base::MutexReadGuard lock(&_mutex);
    lock.Lock();
    std::map<std::string, EventHandler*>::const_iterator it = _name2Handlers.find(name);
    return it == _name2Handlers.end() ? NULL : it->second;
}

int EventHandlerRegistry::RegisterHandler(const std::string& name, EventHandler* handler)
{
    
    base::MutexWriteGuard lock(&_mutex);
    lock.Lock();
    if (_name2Handlers.find(name) != _name2Handlers.end())
    {
        return -1;
    }

    _name2Handlers[name] = handler;
    return 0;
}

void EventHandlerRegistry::RemoveHandler(const std::string& name)
{
    
    base::MutexWriteGuard lock(&_mutex);
    lock.Lock();
    _name2Handlers.erase(name);
}

size_t EventHandlerRegistry::GetCount() const
{
    base::MutexReadGuard lock(&_mutex);
    lock.Lock();
    return _name2Handlers.size();
}

}

