#include <base/thread/thread.h>
#include <pthread.h>

namespace base {

void* __THREAD_FUNC(void* p)
{
	Thread* thread = static_cast<Thread*>(p);
	thread->Run();
	thread->_alive = false;
	return NULL;
}

Thread::Thread()
	: _hdl(0), _alive(false)
{
}

Thread::~Thread()
{
	Stop();
}

int Thread::Start()
{
	if ( _alive )
    {
		return -1;
	}

	pthread_attr_t attr;
	if ( pthread_attr_init(&attr) != 0 ) {
		return -1;
	}
	
	if ( pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE) != 0 ) {
		return -1;
	}
	
	if (pthread_create(&_hdl, &attr, __THREAD_FUNC, this) == -1) {
		return -1;
	}

	_alive = true;
	return 0;
}

void Thread::Stop()
{
	if ( _alive ) {
		pthread_cancel(_hdl);
		_alive = false;
	}
}

void Thread::Join()
{
	if ( _alive ) {
		pthread_join(_hdl, NULL);
		_alive = false;
	}
}

} 

