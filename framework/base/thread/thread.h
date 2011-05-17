#ifndef _BASE_THREAD_H
#define _BASE_THREAD_H

#include <pthread.h>
#include <base/thread/nocopyable.h>

namespace base {

class Thread : public NoCopyable
{
public:
	Thread():_threadHandle(0), _alive(false){};
    virtual ~Thread();

	virtual int Start();
	virtual void Stop();
	void Wait();

	inline bool Alive() const { return _alive; }
	inline pthread_t ThreadId() const { return _threadHandle; }

protected:
	virtual void Run() = 0;	

private:

    static void* ThreadFunc(void*p);
	pthread_t _threadHandle;
	bool _alive;
};
} 

#endif


