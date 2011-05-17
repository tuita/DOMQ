#ifndef _NETMGR_CONNECTIONMGR_H__
#define _NETMGR_CONNECTIONMGR_H__

#include <netmgr/msgcallback.h>
#include <netmgr/handlerregistry.h>
#include <netmgr/message.h>
#include <netmgr/networkoption.h>
#include <base/util/objectqueue.h>
#include <base/util/allocator.h>
#include <base/thread/mutex.h>
#include <base/net/sockaddr.h>
#include <base/thread/thread.h>
#include <base/thread/mutex.h>
#include <base/net/pipeeventhandler.h>
#include <base/config/libconfig.h++>
#include <map>
#include <list>
#include <string>

namespace base {
    class EventManager;
    class Acceptor;
}

namespace netmgr
{

class ConnectionThread: public base::Thread
{
public:
    ConnectionThread();

    void SetReactor(base::EventManager* eventMgr){_eventMgr = eventMgr;}
private:
    void Run();

    base::EventManager* _eventMgr;

};

class ConnectionMgr
{
public:
    ConnectionMgr();

    virtual ~ConnectionMgr();

    virtual int  Open(libconfig::Setting& setting);
    virtual int InitNetwork(libconfig::Setting& setting);
    virtual void Close();

    void Run(bool sync = false);
    void Stop();

    Message* PopMsg(bool block);
    Message* PopMsg(int usBlockTime);
    int PushMsg(Message* msg);

    void SetPackageLimit(int limit){ _resultLimit = limit; }
    void SetQueueLimit(size_t limit){ _requestLimit = limit; }

    int Listen(const NetworkOption& option);

protected:
  
    char* GetBuffer(size_t bufSize=0);
    size_t GetBufferSize();

    int ParseCoder(NetworkOption& option, const libconfig::Setting& setting);

private:

    std::list<base::Acceptor*>  _acceptors;

    base::SyncObjectStack<netmgr::Message*> _requestQueue;
    base::SyncObjectStack<netmgr::Message*> _resultQueue;
    size_t                      _requestLimit;
    size_t                      _resultLimit;
    EventHandlerRegistry        _eventhandleRegistery;
    ExtractMsgCallBack          _extractMsgCallBack;
    PushMsgCallBack             _newMsgCallBack;
    QueuePushMsgCallBack        _queuePushMsgCallBack;
    base::PipeEventHandler     _newMsgEventHandler;

    base::EventManager*        _eventMgr;

    base::GxxMemoryAllocator    _memAllocator;
    /*base::ObjectPoolAllocator<Package> packageAllocator;*/
    ConnectionThread _connectionThread;

    std::vector<netmgr::ICoder*> _coders;
private:

};

}

#endif

