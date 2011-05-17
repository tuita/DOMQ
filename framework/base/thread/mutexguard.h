#ifndef _BASE_MUTEX_GUARD_H_
#define _BASE_MUTEX_GUARD_H_

#include <base/thread/mutex.h>
namespace base {

class MutexGuard
{
public:
    MutexGuard(const IMutex * mutex) : _mutex(mutex), _locked(false) {}
    ~MutexGuard()
    {
        if(_locked) _mutex->Release();
    }

    int Lock(bool block=true) const
    {
        if ( _locked ) return -1;
        if ( _mutex->Acquire(block) != 0 )  return -1; 
        _locked = true;
        return 0;
    }

    int Unlock() const
    {
        if ( !_locked ) return -1; 
        if ( _mutex->Release() != 0 ) return -1; 
        _locked = false;
        return 0;
    }

    bool IsLocked() const
    {
        return _locked;
    }

private:
    const IMutex * _mutex;
    mutable bool _locked;
};

class MutexReadGuard
{
public:

    MutexReadGuard(const IRWMutex * mutex) : mutex_(mutex), _lockRead(false) {}

    ~MutexReadGuard()
    {
        if (_lockRead) mutex_->Release(); 
    }

    int Lock(bool block=true) const
    {
        if ( _lockRead ) return -1; 
        if ( mutex_->AcquireRead(block) != 0 ) return -1; 
        _lockRead = true;
        return 0;
    }

    int Unlock() const
    {
        if ( !_lockRead )  return -1; 
        if ( mutex_->Release() != 0 ) return -1; 
        _lockRead = false;
        return 0;
    }

    bool IsLocked() const
    {
        return _lockRead;
    }

private:
    const IRWMutex * mutex_;
    mutable bool _lockRead;
};

class MutexWriteGuard
{
public:

    MutexWriteGuard(const IRWMutex * mutex) : mutex_(mutex), _writeLock(false) {}

    ~MutexWriteGuard()
    {
        if (_writeLock) mutex_->Release(); 
    }

    int Lock(bool block=true) const
    {
        if ( _writeLock ) return -1; 
        if ( mutex_->AcquireWrite(block) != 0 ) return -1; 
        _writeLock = true;
        return 0;
    }

    int Unlock() const
    {
        if ( !_writeLock )  return -1; 
        if ( mutex_->Release() != 0 ) return -1; 
        _writeLock = false;
        return 0;
    }

    bool IsLocked() const
    {
        return _writeLock;
    }

private:
    const IRWMutex * mutex_;
    mutable bool _writeLock;
};

}

#endif
