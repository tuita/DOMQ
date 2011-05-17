#ifndef _MUTEX_GUARD_H_
#define _MUTEX_GUARD_H_

#include <base/thread/mutex.h>
namespace base {

class LockGuard
{
public:

    LockGuard(const IMutex * mutex) : _mutex(mutex), _acquired(false) {}
    ~LockGuard()
    {
        if (_acquired) {
            _mutex->Release();
        }
    }

    int Lock(bool block=true) const
    {
        if ( _acquired ) {
            return -1;
        }

        if ( _mutex->Acquire(block) != 0 ) {
            return -1;
        }

        _acquired = true;
        return 0;
    }

    int Unlock() const
    {
        if ( !_acquired ) {
            return -1;
        }
        if ( _mutex->Release() != 0 ) {
            return -1;
        }
        _acquired = false;
        return 0;
    }

    bool IsLocked() const
    {
        return _acquired;
    }

private:
    const IMutex * _mutex;
    mutable bool _acquired;
};

class RLockGuard
{
public:

    RLockGuard(const IRWMutex * mutex) : mutex_(mutex), acquired_(false) {}

    ~RLockGuard()
    {
        if (acquired_) {
            mutex_->Release();
        }
    }

    int Lock(bool block=true) const
    {
        if ( acquired_ ) {
            return -1;
        }

        if ( mutex_->AcquireRead(block) != 0 ) {
            return -1;
        }

        acquired_ = true;
        return 0;
    }

    int Unlock() const
    {
        if ( !acquired_ ) {
            return -1;
        }
        if ( mutex_->Release() != 0 ) {
            return -1;
        }
        acquired_ = false;
        return 0;
    }

    bool IsLocked() const
    {
        return acquired_;
    }

private:
    const IRWMutex * mutex_;
    mutable bool acquired_;
};

class WLockGuard
{
public:

    WLockGuard(const IRWMutex * mutex) : mutex_(mutex), acquired_(false) {}

    ~WLockGuard()
    {
        if (acquired_) {
            mutex_->Release();
        }
    }

    int Lock(bool block=true) const
    {
        if ( acquired_ ) {
            return -1;
        }

        if ( mutex_->AcquireWrite(block) != 0 ) {
            return -1;
        }

        acquired_ = true;
        return 0;
    }

    int Unlock() const
    {
        if ( !acquired_ ) {
            return -1;
        }
        if ( mutex_->Release() != 0 ) {
            return -1;
        }
        acquired_ = false;
        return 0;
    }

    bool IsLocked() const
    {
        return acquired_;
    }

private:
    const IRWMutex * mutex_;
    mutable bool acquired_;
};

}

#endif
