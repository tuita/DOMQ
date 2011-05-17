#ifndef _NETMGR_PUSHMSGCALLBACK_H_
#define _NETMGR_PUSHMSGCALLBACK_H_

#include <base/util/objectqueue.h>

namespace netmgr
{
class EventHandlerRegistry;
class Message;

class QueuePushMsgCallBack : public base::CallBack
{
public:
    void SetPipe(int fd){ _fd = fd;}
    int Call(void* p);

private:
    int _fd;

};

class PushMsgCallBack : public base::CallBack
{
public:
	void SetHandlerRegistry(EventHandlerRegistry* registry)
	{   this->_registry = registry; }

	void SetQueue(base::ObjectQueue<Message*>* queue)
	{   this->_queue = queue; }

	virtual int Call(void*p);

protected:
    EventHandlerRegistry* _registry;
    base::ObjectQueue<Message*>* _queue;
};

} // namespace basen

#endif // ACN_PUSHEVENTHANDLER_H_
