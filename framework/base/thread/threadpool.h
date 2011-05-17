#ifndef AC_UTIL_THREADPOOL_H_
#define AC_UTIL_THREADPOOL_H_

#include <base/thread/nocopyable.h>
#include <base/thread/thread.h>
#include <base/thread/mutex.h>
#include <pthread.h>
#include <list>

namespace base {

class ThreadPool  : public NoCopyable
{
protected:
    class WorkerThread : public Thread
    {
    private:
        ThreadPool* pool;
    public:
        WorkerThread(ThreadPool* pool);
        void Run();
    };
public:
    ThreadPool():_stop(false){}
    virtual ~ThreadPool(){}
	int Start(size_t count);
	void StopAll();
	size_t GetThreadNum();
    bool Stop(){ return _stop;}

protected:
	virtual bool RunOnce() = 0;
	Thread* Create();
    void Destory(const Thread* thread);

protected:
    typedef std::list<Thread*>	ThreadContainer;
    ThreadContainer	_livethreads;
    base::ThreadMutex _mutex;
    bool _stop;
};

}

#endif

