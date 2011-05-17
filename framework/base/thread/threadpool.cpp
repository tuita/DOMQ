#include <base/thread/threadpool.h>
#include <base/thread/mutexguard.h>
#include <pthread.h>
#include <unistd.h>
#include <algorithm>
#include <cassert>


namespace base {




ThreadPool::WorkerThread::WorkerThread(ThreadPool* pool_): pool(pool_)
{
}

void ThreadPool::WorkerThread::Run()
{
	while(!pool->Stop() && pool->RunOnce());
	pool->Destory(this);
}

Thread* ThreadPool::Create()
{
	return new WorkerThread(this);
}

void ThreadPool::Destory(const Thread* thread)
{
    base::MutexGuard guard(&_mutex);
    guard.Lock();
    ThreadContainer::iterator it = std::find(_livethreads.begin(), _livethreads.end(), thread);
    assert(it != _livethreads.end());
    _livethreads.erase(it);
}


int ThreadPool::Start(size_t count)
{
	size_t livecount = _livethreads.size();
	if ( livecount < count )
	{
		while( livecount < count )
		{
			Thread* thread = Create();
			if (thread->Start() != 0)
			{
				return -1;
			}
			_livethreads.push_back(thread);
			++livecount;
		}
	}
	return 0;
}

void ThreadPool::StopAll()
{
	_stop = true;
}

size_t ThreadPool::GetThreadNum()
{
	return _livethreads.size();
}

}

