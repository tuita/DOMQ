#ifndef ADDRESS_FILTER_ACCEPTOR_H_
#define ADDRESS_FILTER_ACCEPTOR_H_

#include <netmgr/eventhandler.h>
#include <base/net/acceptor.h>
#include <base/util/allocator.h>
#include <base/util/dispose.h>

#include <list>
#include <string>
namespace base
{
class SockAddress;
}

namespace netmgr
{

class EventHandlerRegistry;

class Acceptor : public base::Acceptor
{
public:
    Acceptor(const base::SockAddress& bindaddr, const NetworkOption& option,  EventHandlerRegistry* registry);
    void SetNetworkOption(const NetworkOption& option) { _option = option; }
    const NetworkOption& GetNetworkOption() {return _option; }

protected:
    virtual base::EventHandler* CreateHandler(const base::SockAddress& addr);
    virtual void DestroyHandler(base::EventHandler* handler);



private:
    NetworkOption _option;
    base::ObjectPoolAllocator<NetMgrEventHandler> _eventHandlerAllocator;
    EventHandlerRegistry*  _eventHandlerRegistry;
    
    
};

} 

#endif 

