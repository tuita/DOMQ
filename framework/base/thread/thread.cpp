#include <base/thread/thread.h>
#include <pthread.h>

namespace base {

Thread::~Thread()
{
	Stop();
}

int Thread::Start()
{
	if ( _alive ) return -1; 

	pthread_attr_t attr;
	if ( pthread_attr_init(&attr) != 0 )  return -1; 
	if ( pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE) != 0 )  return -1; 
    if (pthread_create(&_threadHandle, &attr, &Thread::ThreadFunc, this) == -1)  return -1; 
	_alive = true;
	return 0;
}

void Thread::Stop()
{
	if ( _alive ) {
		pthread_cancel(_threadHandle);
		_alive = false;
	}
}

void Thread::Wait()
{
	if ( _alive ) {
		pthread_join(_threadHandle, NULL);
		_alive = false;
	}
}

void* Thread::ThreadFunc(void* p)
{
    Thread* thread = (Thread*)p;
    thread->Run();
    thread->_alive = false;
    return NULL;
}
} 

