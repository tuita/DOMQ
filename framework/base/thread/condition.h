#ifndef _BASE_CONDITION_H_
#define _BASE_CONDITION_H_

#include <unistd.h>
#include <pthread.h>
#include <base/thread/mutex.h>

namespace base
{

class ICondition
{
public:
    typedef IMutex MutexType;

public:
	virtual ~ICondition() {}
	virtual int Wait(const struct timespec * pTimeout=NULL) const = 0;
	virtual int Signal() const = 0;
	virtual int Broadcast() const = 0;
};


class NilMutexCond : public ICondition
{
public:
    typedef NilMutex MutexType;

public:
    NilMutexCond(const base::NilMutex & cMutex = NilMutex());
	virtual int Wait(const struct timespec * = NULL) const { usleep(20); return 0; }	
	virtual int Signal() const { return 0; }
	virtual int Broadcast() const { return 0; }
public:
	static NilMutexCond * Instance();	
};

class ThreadCond : public ICondition
{
public:
    typedef ThreadMutex MutexType;

public:
	ThreadCond(const ThreadMutex & cThreadMutex);
	~ThreadCond();

	virtual int Wait(const struct timespec * pTimeout=NULL) const;
	virtual int Signal() const;
	virtual int Broadcast() const;

private:
	const ThreadMutex & _mutex;
	mutable pthread_cond_t _threadCond;
};

}

#endif

