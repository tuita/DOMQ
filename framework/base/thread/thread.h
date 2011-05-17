#ifndef _BASE_THREAD_H
#define _BASE_THREAD_H

#include <pthread.h>
#include <base/thread/nocopyable.h>

namespace base {

class Thread : public NoCopyable
{
public:
	Thread();
	virtual ~Thread();
	virtual int Start();
	virtual void Stop();
	void Join();

	inline bool IsAlive() const { return _alive; }
	inline pthread_t ThreadId() const { return _hdl; }

private:
	virtual void Run() = 0;	

private:
	pthread_t _hdl;
	bool _alive;

	friend void* __THREAD_FUNC(void* p);
};
} 

#endif


