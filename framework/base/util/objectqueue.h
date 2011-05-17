#ifndef _BASE_OBJECT_QUEUE_H_
#define _BASE_OBJECT_QUEUE_H_
#include <pthread.h>
#include <errno.h>
#include <list>
#include <time.h>
#include <base/util/callback.h>
#include <ext/pool_allocator.h>

namespace base {

template<typename T>
class ObjectQueue
{
public:
    ObjectQueue() : _pushCallBack(NULL), _popCallBack(NULL) {}

    virtual ~ObjectQueue() {}

    void SetPushCallBack(CallBack* notify) { this->_pushCallBack = notify; }

    void SetPopCallBack(CallBack* notify) { this->_popCallBack = notify; } 


    int Push(const T& value, bool limit=true)
    {
        int ret = PushImp(value, limit);
        if ( ret == 0 && _pushCallBack) 
        {
            _pushCallBack->Call(this);
        }
        return ret;
    }

    int Pop(T& value, int usBlockTime=-1)
    {
        int ret = PopImp(value, usBlockTime);
        if(0 == ret && _popCallBack)
        {
            _popCallBack->Call(this);
        }
        return ret;
    }
    
    virtual size_t GetSize() const = 0;    

protected:

    virtual int PushImp(const T& value, bool limit=true) = 0;
    
    virtual int PopImp(T& value, int usBlockTime) = 0;

private:
    CallBack* _pushCallBack;
    CallBack* _popCallBack;
};


template<class T>
class SyncObjectQueue : public ObjectQueue<T>
{
public:
    enum { DEFAULT_LIMIT = 1000 };

public:
    SyncObjectQueue(size_t limit = DEFAULT_LIMIT) 
        : _max(limit), _count(0)
    {
        pthread_mutex_init(&_mutex, NULL);
        pthread_cond_init(&_cond, NULL);
    }

    virtual ~SyncObjectQueue()
    {
        pthread_cond_destroy(&_cond);
        pthread_mutex_destroy(&_mutex);
    }

    virtual size_t GetSize() const
    {
        return _count;
    }

    size_t GetLimit() const
    {
        return _max;
    }

    void SetLimit(size_t limit)
    {
        _max = limit;
    }

protected:

    virtual int PushImp(const T& value, bool limit=true)
    {
        pthread_mutex_lock(&_mutex);

        if ( limit && _count >= _max ) {
            pthread_mutex_unlock(&_mutex);            
            return -1;
        }
            
        _data.push_back(value);
        _count ++;
        pthread_cond_signal(&_cond);
        pthread_mutex_unlock(&_mutex);

        return 0;
    }

    virtual int PopImp(T& value, int usBlockTime)
    {
        int tmpUsBlockTime = usBlockTime >= 0 ? usBlockTime : 1000*1000;
        pthread_mutex_lock(&_mutex);
        while(true)
        {
            if (!_data.empty())
            {
                value = _data.front();
                _data.pop_front();
                _count --;
                pthread_mutex_unlock(&_mutex);
                return 0;
            }

            if (0 == usBlockTime)
            {
                pthread_mutex_unlock(&_mutex);
                return -1;
            }
            else
            {
                timespec timeout;
                clock_gettime(CLOCK_REALTIME, &timeout);
                size_t us = timeout.tv_nsec/1000+tmpUsBlockTime ; 
                timeout.tv_sec  += us/(1000*1000);
                timeout.tv_nsec = (us%(1000*1000))*1000;
                if(ETIMEDOUT == pthread_cond_timedwait(&_cond, &_mutex, &timeout) && usBlockTime>=0)
                {
                    pthread_mutex_unlock(&_mutex);
                    return -1;
                }
            }
        }
    }

protected:

    mutable pthread_mutex_t _mutex;
    mutable pthread_cond_t  _cond;

    std::list<T, __gnu_cxx::__pool_alloc<T> > _data;
    size_t    _max;    
    size_t    _count;
};

template <class T>
class SyncObjectStack : public SyncObjectQueue<T>
{
protected:
    virtual int PushImp(const T& value, bool limit=true)
    {
        pthread_mutex_lock(&this->_mutex);
        
        if ( limit && this->_count >= this->_max ) 
        {
            pthread_mutex_unlock(&this->_mutex);
            return -1;
        }
            
        this->_data.push_front(value);
        this->_count ++;
        pthread_cond_signal(&this->_cond);
        pthread_mutex_unlock(&this->_mutex);
        return 0;
    }
};

} // namespace base


#endif // AC_UTIL_QUEUE_H_

