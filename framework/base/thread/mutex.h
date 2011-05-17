#ifndef _BASE_MUTEX_H_
#define _BASE_MUTEX_H_

#include <pthread.h>
#include <sys/sem.h>

namespace base
{

class IMutex
{
public:
	virtual ~IMutex() {}
	virtual int Acquire(bool block=true) const = 0;
	virtual int Release() const = 0;
};

class IRWMutex
{
public:
    virtual ~IRWMutex() {}

    virtual int AcquireRead(bool block=true) const = 0;
    virtual int AcquireWrite(bool block=true) const = 0;
    virtual int Release() const = 0;
};


class NilMutex : public IMutex
{
public:
	virtual int Acquire(bool block=true) const { (void)block; return 0; }
	virtual int Release() const { return 0; }
public:
	static NilMutex * Instance();
};


class ThreadMutex : public IMutex
{
public:
	ThreadMutex()
    {
        pthread_mutex_init(&mutex_, NULL);
    }

	virtual ~ThreadMutex()
    {
        pthread_mutex_destroy(&mutex_);
    }
	
	virtual int Acquire(bool block=true) const
    { 
        return block ? pthread_mutex_lock(&mutex_) : pthread_mutex_trylock(&mutex_); 
    }

	virtual int Release() const
    {
        return pthread_mutex_unlock(&mutex_);
    }

private:
	mutable pthread_mutex_t mutex_;

	friend class ThreadCond;
};

class ThreadRWMutex : public IRWMutex
{
public:
    ThreadRWMutex()
    {
        pthread_rwlock_init(&_mutex, NULL);
    }

    ~ThreadRWMutex()
    {
        pthread_rwlock_destroy(&_mutex);
    }

    int AcquireRead(bool block=true) const
    {
        return block ? pthread_rwlock_rdlock(&_mutex) : pthread_rwlock_tryrdlock(&_mutex);
    }

    int AcquireWrite(bool block=true) const
    {
        return block ? pthread_rwlock_wrlock(&_mutex) : pthread_rwlock_trywrlock(&_mutex);
    }

    int Release() const
    {
        return pthread_rwlock_unlock(&_mutex);
    }

private:
    mutable pthread_rwlock_t _mutex;
};

}

#endif

