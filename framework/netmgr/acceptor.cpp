#include <netmgr/acceptor.h>
#include <netmgr/eventhandler.h>
#include <netmgr/handlerregistry.h>
#include <base/log/log.h>
#include <cassert>
#include <errno.h>
#include <arpa/inet.h>
#include <base/net/sockaddr.h>


#define MAX_CONNECTION 10240 

namespace netmgr
{

Acceptor::Acceptor(const base::SockAddress& bindaddr, const NetworkOption& option, EventHandlerRegistry* registry) 
: base::Acceptor(bindaddr), _option(option),     
      _eventHandlerAllocator(128, MAX_CONNECTION, 128),
      _eventHandlerRegistry(registry)
{
    
}

base::EventHandler* Acceptor::CreateHandler(const base::SockAddress& addr)
{
    NetMgrEventHandler* handler = _eventHandlerAllocator.Create();
    if ( handler != NULL ) 
    {
        _eventHandlerRegistry->RegisterHandler(addr.ToString(), handler);
        handler->SetPeerAddr(addr);
        handler->SetNetworkOption(&_option);
    }

    return handler;
}

void Acceptor::DestroyHandler(base::EventHandler* handler)
{
    assert(handler);
    NetMgrEventHandler* netmgrHandler = static_cast<NetMgrEventHandler*>(handler);
    _eventHandlerRegistry->RemoveHandler(netmgrHandler->GetPeerAddr().ToString());
    _eventHandlerAllocator.Destroy(netmgrHandler);

}

} 

