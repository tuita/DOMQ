#include <base/util/allocator.h>
#include <sys/types.h>
#include <stdlib.h>
#include <base/thread/mutex.h>

namespace base
{
void * GxxMemoryAllocator::Create(size_t iSize, size_t * pRealSize)
{
	
	if ( pRealSize != NULL ) {
		*pRealSize = iSize;
	}	
	return ::operator new(iSize, ::std::nothrow_t());
}
        
void GxxMemoryAllocator::Destroy(void * pData)
{
	
	return ::operator delete(pData, ::std::nothrow_t());
}

GxxMemoryAllocator * GxxMemoryAllocator::Instance()
{
	static GxxMemoryAllocator oNewAllocator;
	return &oNewAllocator;
}

}

