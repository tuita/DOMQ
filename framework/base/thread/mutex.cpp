#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <base/thread/mutex.h>

namespace base
{

NilMutex * NilMutex::Instance()
{
	static NilMutex oNullMutex;
	return &oNullMutex;
}

ThreadMutex::ThreadMutex()
{
	pthread_mutex_init(&mutex_, NULL);
}
	
ThreadMutex::~ThreadMutex()
{
	pthread_mutex_destroy(&mutex_);
}
	
int ThreadMutex::Acquire(bool block) const
{
	if ( block ) {
		return pthread_mutex_lock(&mutex_);
	}
	else {
		return pthread_mutex_trylock(&mutex_);
	}
}
	
int ThreadMutex::Release() const
{
	return pthread_mutex_unlock(&mutex_);
}

}

