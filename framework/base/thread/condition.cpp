#include <pthread.h>
#include <base/thread/mutex.h>
#include <base/thread/condition.h>

using namespace base;

namespace base{

NilMutexCond::NilMutexCond(const NilMutex&)
{
}

NilMutexCond * NilMutexCond::Instance()
{
    static NilMutexCond oNullMutexCond;
	return &oNullMutexCond;
}


ThreadCond::ThreadCond(const ThreadMutex & cThreadMutex)
	: _mutex(cThreadMutex)
{
	pthread_cond_init(&_threadCond, NULL);
}

ThreadCond::~ThreadCond()
{
	pthread_cond_destroy(&_threadCond);
}

int ThreadCond::Wait(const struct timespec * pTimeout) const
{
	if ( pTimeout == NULL ) {
		if ( pthread_cond_wait(&_threadCond, &_mutex.mutex_) != 0 ) {
			return -1;
		}
	}
	else {
		if ( pthread_cond_timedwait(&_threadCond, &_mutex.mutex_, pTimeout) != 0 ) {
			return -1;
		}
	}
	return 0;
}

int ThreadCond::Signal() const
{
	return pthread_cond_signal(&_threadCond);
}

int ThreadCond::Broadcast() const
{
	return pthread_cond_broadcast(&_threadCond);
}
}

