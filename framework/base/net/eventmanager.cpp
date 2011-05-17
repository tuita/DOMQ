#include <base/net/eventmanager.h>
#include <base/net/eventhandler.h>
#include <base/log/log.h>
#include <event.h>
#include <cassert>
#include <string.h>
#include <new>
#include <list>

namespace base {

struct TimerData
{
	event	timer;
	void *	argv[2];

	TimerData()
	{
		memset(&timer, 0, sizeof(timer));
		memset(argv, 0, sizeof(argv));
	}
};

struct EventData
{
	event			readev;
	event			writeev;
	std::list<TimerData*>	timers;

	EventData()
	{
		memset(&readev, 0, sizeof(readev));
		memset(&writeev, 0, sizeof(writeev));
	}
};

void __EventCallBack(int/* fd*/, short events, void *arg)
{
	EventHandler* handler = static_cast<EventHandler*>(arg);
	assert(handler);

	if (events & EV_TIMEOUT) 
    {
		handler->HandleTimeout(NULL);
	}
	else if (events & EV_READ) 
    {
		handler->HandleInput();
	}
	else if (events & EV_WRITE) 
    {
		handler->HandleOutput();
	}
}

void __TimeEventCallBack(int/* fd*/, short events, void *arg)
{
	if ( ! (events & EV_TIMEOUT) ) 
    {
		return;
	}

	void ** argv = static_cast<void **>(arg);
	EventHandler* handler = static_cast<EventHandler*>(argv[0]);
	TimerData* pTimerData = static_cast<TimerData*>(argv[1]);
	handler->HandleTimeout(pTimerData);
	handler->GetEventManager()->RemoveTimeHandler(handler, pTimerData);
}

EventManager::EventManager(IMemoryAllocator* allocator)
: _allocator(allocator)
{
	_eventBase = static_cast<event_base*>(event_init());
}

EventManager::~EventManager()
{
}

int EventManager::initEventHandler(EventHandler* handler)
{
	if(handler->Context() == NULL ) 
    {
		EventData* eventdata = AllocEventData();
		if ( eventdata == NULL ) 
        {
			return -1;
		}
		handler->Context(eventdata);
	}

	handler->SetEventManager(this);	
	return 0;
}

void EventManager::uninitEventHander(EventHandler* handler, EventData* ed)
{
	ReleaseEventData(ed);
	handler->Context(NULL);
	handler->HandleClose();		
}

bool EventManager::containEvent(EventData* eventdata)
{
	return (eventdata->readev.ev_events | eventdata->writeev.ev_events) || !eventdata->timers.empty();
}

int EventManager::RegisterHandler(int mask, EventHandler* handler, timeval* timeout)
{
	initEventHandler(handler);
	EventData* ed = static_cast<EventData*>(handler->Context());
	assert(ed!=NULL);

	short events = 0;
	if (mask & ReadMask)
	{
		events |= EV_READ;
		if (timeout) events |= EV_TIMEOUT;
		if (mask & PersistMask)	events |= EV_PERSIST;
		event_del(&ed->readev);
		event_set(&ed->readev, handler->GetEventObject().GetObject(), events, __EventCallBack, handler);
		event_base_set(_eventBase, &ed->readev);
		if (event_add(&ed->readev, timeout) != 0)
        {
			return -1;
		}
	}

	events = 0;
	if (mask & WriteMask)
	{
		events |= EV_WRITE;
		if (timeout) events |= EV_TIMEOUT;
		if (mask & PersistMask) events |= EV_PERSIST;		
		event_del(&ed->writeev);
		event_set(&ed->writeev, handler->GetEventObject().GetObject(), events, __EventCallBack, handler);
		event_base_set(_eventBase, &ed->writeev);
		if (event_add(&ed->writeev, timeout) != 0) 
        {
			return -1;
		}
	}

	return 0;	
}

int EventManager::RemoveHandler(int mask, EventHandler* handler)
{
	EventData* ed = static_cast<EventData*>(handler->Context());
	if (!ed)
	{
		handler->HandleClose();
		return 0;
	}

	if (mask & ReadMask)
	{
		event_del(&ed->readev);
		memset(&ed->readev, 0, sizeof(ed->readev));
	}
	if (mask & WriteMask)
	{
		event_del(&ed->writeev);
		memset(&ed->writeev, 0, sizeof(ed->writeev));
	}
	if ( !containEvent(ed) ) 
    {
		uninitEventHander(handler, ed);
	}
	return 0;
}

int EventManager::RegisterTimeHandler(EventHandler* handler, timeval* timeout, void ** pTimerID)
{
	if ( timeout == NULL ) return -1;
	initEventHandler(handler);
	EventData* ed = static_cast<EventData*>(handler->Context());
	TimerData * pTimerData = new TimerData;
	if ( pTimerData == NULL ) return -1;

    pTimerData->argv[0] = handler;
	pTimerData->argv[1] = pTimerData;
	
	evtimer_set(&pTimerData->timer, __TimeEventCallBack, &pTimerData->argv);
	event_base_set(_eventBase, &pTimerData->timer);
	if ( evtimer_add(&pTimerData->timer, timeout) != 0 ) 
    {
		delete pTimerData;
		return -1;
	}

	ed->timers.push_back(pTimerData);
	if ( pTimerID ) 
    {
		*pTimerID = pTimerData;
	}
	
	return 0;
}

int EventManager::RemoveTimeHandler(EventHandler* handler, void * timeid)
{
	EventData* eventdata = static_cast<EventData*>(handler->Context());
	if (!eventdata)
	{
		handler->HandleClose();
		return 0;
	}

	if ( timeid == NULL ) 
    {
		for(std::list<TimerData *>::const_iterator it=eventdata->timers.begin(); it!=eventdata->timers.end(); ++it ) 
        {
			event_del(&(*it)->timer);
			delete *it;
		}
		eventdata->timers.clear();
	}
	else 
    {
		TimerData* pTimerData = static_cast<TimerData *>(timeid);
        std::list<TimerData*>::iterator pos;
		for(pos = eventdata->timers.begin(); pos!=eventdata->timers.end(); ++pos ) 
        {
			if ( *pos == pTimerData ) 
            {
				break;
			}
		}
		if( pos == eventdata->timers.end() ) 
        {
			return -1;
		}

		if ( event_del(&pTimerData->timer) != 0 ) 
        {
			return -1;
		}
		eventdata->timers.erase(pos);
		delete pTimerData;
	}

	if(!containEvent(eventdata) ) 
    {
		uninitEventHander(handler, eventdata);
	}

	return 0;
}

int EventManager::RunOnce()
{
	return event_base_loop(_eventBase, EVLOOP_ONCE);
}

EventData* EventManager::AllocEventData()
{
	void *p = NULL;
	if (_allocator)
		p = _allocator->Create(sizeof(EventData));
	else
		p = malloc(sizeof(EventData));

	return new(p) EventData;
}

void EventManager::ReleaseEventData(EventData* data)
{
	data->~EventData();
	if (_allocator)
		_allocator->Destroy(data);
	else
		free(data);
}

void EventManager::Run()
{
	event_base_dispatch(_eventBase);
}

void EventManager::Stop()
{
	event_base_loopexit(_eventBase, 0);
}

}

