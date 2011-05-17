#ifndef _BASE_ALLOCATOR_H_
#define _BASE_ALLOCATOR_H_

#include <sys/types.h>
#include <stdlib.h>
#include <base/thread/mutex.h>
#include <base/thread/mutexguard.h>
#include <list>
#include <vector>

namespace base
{

class IMemoryAllocator
{
public:
	virtual ~IMemoryAllocator() {}
	virtual void * Create(size_t iSize, size_t * pRealSize=NULL) = 0;
	virtual void Destroy(void * pData) = 0;
};

template <typename T>
class ObjectAllocator
{
public:
    virtual ~ObjectAllocator() {}
    virtual T * Create() = 0;
    virtual void Destroy(T * pObject) = 0;
};


class GxxMemoryAllocator : public IMemoryAllocator
{
public:
	virtual void * Create(size_t iSize, size_t * pRealSize=NULL);	
	virtual void Destroy(void * pData);
public:
	static GxxMemoryAllocator * Instance();	
};

template <typename T>
class ObjectDefaultAllocator : public ObjectAllocator<T>
{
public:
    ObjectDefaultAllocator(IMemoryAllocator * pAllocator) : _memoryAllocator(pAllocator) {}

    virtual T * Create()
    {
        void * obj = _memoryAllocator->Create(sizeof(T));
        if ( obj == NULL ) {
            return NULL;
        }
        return new (obj) T;
    }

    virtual void Destroy(T * obj)
    {
        obj->~T();
        _memoryAllocator->Destroy(obj);		
    }

private:
    IMemoryAllocator * _memoryAllocator;
};

template <class T>
class ObjectPoolAllocator : public ObjectAllocator<T>
{
public:

    ObjectPoolAllocator(size_t iInitNum, size_t iMaxNum=0, size_t iIncNum=0, IMutex * pMutex=NilMutex::Instance(), IMemoryAllocator *pAllocator=GxxMemoryAllocator::Instance())
        : _allocator(pAllocator), _mutex(pMutex), _currentNum(0), _maxNum(iMaxNum), _onceIncNum(iIncNum)
    {
        if ( _maxNum < iInitNum ) {
            _maxNum = iInitNum;
            _onceIncNum = 0;
        }

        inc(iInitNum);
    }

    virtual ~ObjectPoolAllocator()
    {
        for ( typename::std::list<T *>::iterator pos=_freeObject.begin(); pos!=_freeObject.end(); ++pos ) {
            (*pos)->~T();
        }

        for ( typename::std::vector<T *>::iterator pos=_objChunks.begin(); pos!=_objChunks.end(); ++pos ) {
            _allocator->Destroy(*pos);
        }
    }

    inline void SetMaxNum(size_t iMaxNum)
    {
        if ( _currentNum >= iMaxNum ) {
            _maxNum = _currentNum;
        }
        else {
            _maxNum = iMaxNum;
        }
    }

    inline void SetIncNum(size_t iIncNum)
    {
        _onceIncNum = iIncNum;
    }

    virtual T * Create()
    {
        base::LockGuard lock(_mutex);
        if ( lock.Lock(true) != 0 ) {
            return NULL;
        }

        if ( _freeObject.empty() ) {
            if ( inc(_onceIncNum) != 0 ) {
                return NULL;
            }
        }

        T * pObject = *(_freeObject.begin());
        _freeObject.pop_front();

        return pObject;
    }

    virtual void Destroy(T * pObject)
    {
        LockGuard lock(_mutex);
        if ( lock.Lock(true) != 0 ) {
            return;
        }

        if ( pObject != NULL ) {
            _freeObject.push_front(pObject);
        }
    }

private:

    int inc(size_t iIncNum)
    {
        if ( _currentNum >= _maxNum ) {
            return -1;
        }

        if ( _currentNum + iIncNum > _maxNum ) {
            iIncNum = _maxNum - _currentNum;
        }

        T * pChunk = (T *)_allocator->Create(sizeof(T)*iIncNum);
        if ( pChunk == NULL ) {
            return -1;
        }

        for ( size_t i=0; i<iIncNum; ++i ) {
            T* p = new((void *)(pChunk+i)) T;
            _freeObject.push_back(p);
        }

        _objChunks.push_back(pChunk);
        _currentNum += iIncNum;		
        return 0;		
    }

private:

    IMemoryAllocator *	_allocator;
    IMutex *		_mutex;

    std::list<T *>	_freeObject;
    std::vector<T *>	_objChunks;

    size_t		_currentNum;
    size_t		_maxNum;
    size_t		_onceIncNum;
};


}

#endif // AC_ALLOCATOR_H_

