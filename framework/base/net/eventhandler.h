#ifndef AC_NETWORK_EVENTHANDLER_H_
#define AC_NETWORK_EVENTHANDLER_H_

#include <base/net/eventobject.h>

namespace base {

class IEventManager;

enum EventMask
{
    ReadMask	= 0x1,
    WriteMask	= 0x2,
    PersistMask = 0x4
};

class EventHandler 
{
public:
	EventHandler() : _eventMgr(0), _context(0) {}
	virtual ~EventHandler() {}


	void SetEventObject(EventObject obj) { this->_eventObj = obj; }

	EventObject& GetEventObject() { return this->_eventObj; }

	void SetEventManager(IEventManager* evMgr)	{ this->_eventMgr = evMgr; }

	IEventManager* GetEventManager() const { return this->_eventMgr; }


    void Context(void* data){ this->_context = data; }

	void* Context() const { return _context; }

	virtual int Open() = 0 ;
	virtual int Close() = 0 ;

	virtual void HandleInput() = 0 ;
	virtual void HandleOutput() = 0 ;
	virtual void HandleTimeout(void * pTimerID) = 0 ;
	virtual void HandleClose() = 0;

protected:
    EventObject		_eventObj;
    IEventManager*	_eventMgr;
    void*		_context;
};

} // namespace base

#endif // AC_NETWORK_EVENTHANDLER_H_

