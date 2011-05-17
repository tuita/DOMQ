#include <base/thread/threadpool.h>
#include <pthread.h>
#include <unistd.h>
#include <algorithm>

namespace base {

class JobThread : public Thread
{
private:
	ThreadPool* pool;

public:
	JobThread(ThreadPool* pool);

	virtual void Run();
};


JobThread::JobThread(ThreadPool* pool_)
: pool(pool_)
{
	
}

void JobThread::Run()
{
	while(true)
	{
		bool ret = pool->RunOnce();
		if (!ret) // need to exit
		{
			break;
		}
	}

	pool->DestroyThread(this);
}

ThreadPool::ThreadPool() : stopcount(0)
{
	
	pthread_mutex_init(&mutex, NULL);
}

ThreadPool::~ThreadPool()
{
	
	pthread_mutex_destroy(&mutex);
}

Thread* ThreadPool::CreateThread()
{
	

	return new JobThread(this);
}

void ThreadPool::DestroyThread(Thread* thread)
{

	pthread_mutex_lock(&mutex);
	ThreadPtrContainer::iterator iter = find(livethreads.begin(), livethreads.end(), thread);
	if ( iter != livethreads.end() ) {
		livethreads.erase(iter);
	}

	deadthreads.push_back(thread);
	pthread_mutex_unlock(&mutex);
}

int ThreadPool::Start(size_t count)
{
	

	size_t livecount = livethreads.size();

	if ( livecount < count )
	{
		while( livecount < count )
		{
			Thread* thread = CreateThread();
			if (thread->Start() != 0)
			{
				return -1;
			}
			livethreads.push_back(thread);
			++livecount;
		}
	}
	else if ( livecount > count ) {
		Stop(livecount-count);
	}

	return 0;
}

void ThreadPool::Stop(size_t count)
{
	

	if ( count == 0 ) {
		return;
	}

	size_t stopcount = ::std::min(count, livethreads.size());

	StopThreads(stopcount);

	// wait for exiting of all stopcount thread
	while( true ) {

		usleep(20);

		pthread_mutex_lock(&mutex);
		if ( deadthreads.size() >= stopcount || livethreads.empty() ) {
			pthread_mutex_unlock(&mutex);
			break;
		}
		pthread_mutex_unlock(&mutex);
	}

	for ( ThreadPtrContainer::iterator iter=deadthreads.begin(); iter!=deadthreads.end(); ++iter ) {
		(*iter)->Join();
		delete *iter;
	}

	deadthreads.clear();
	return;
}

void ThreadPool::StopAll()
{
	

	Stop(livethreads.size());
}

size_t ThreadPool::GetThreadNum()
{
	

	return livethreads.size();
}


bool ThreadPool::RunOnce()
{
	

	while ( true ) {

		usleep(1000000);

		if ( stopcount > 0 ) {

			pthread_mutex_lock(&mutex);

			if ( this->stopcount > 0 ) {
				--stopcount;
				pthread_mutex_unlock(&mutex);
				break;
			}

			pthread_mutex_unlock(&mutex);
		}
	}

	return false;
}

void ThreadPool::StopThreads(size_t stopcount)
{
	

	this->stopcount = stopcount;
}

}

