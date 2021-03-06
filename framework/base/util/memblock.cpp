#include <base/util/memblock.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <sys/types.h>
#include <base/util/allocator.h>

namespace base {

MemBlock::MemBlock(size_t iSize, IMemoryAllocator *pAllocator)
	: _allocator(pAllocator), _begin(0), _end(0), _blockEnd(0)
{
	

	size_t realSize=0;
	_begin = (char*)_allocator->Create(iSize, &realSize);
	if ( _begin == NULL ) 
    {
		return;
	}
	_end = _begin;
	_blockEnd = _begin + realSize;
}

MemBlock::MemBlock(const MemBlock & memBlock)
{
	_allocator = memBlock._allocator;
	size_t iCapacity = memBlock.TotalSpace();
	_begin = (char*)_allocator->Create(iCapacity);
	if ( _begin == NULL ) 
    {
		return;
	}

	_end = _begin;
	_blockEnd = _begin + iCapacity;
	Append(memBlock._begin, memBlock.Size());
}

MemBlock::~MemBlock()
{
	if ( _begin ) 
    {
		_allocator->Destroy(_begin);
	}
}

//MemBlock & MemBlock::operator=(const MemBlock & memBlock)
//{
//	MemBlock memBlockTmp(memBlock);
//	this->Swap(memBlockTmp);
//	return *this;
//}

void MemBlock::Swap(MemBlock & memBlock)
{
	::std::swap(_allocator, memBlock._allocator);
	::std::swap(_begin, memBlock._begin);
	::std::swap(_end, memBlock._end);
	::std::swap(_blockEnd, memBlock._blockEnd);
}

int MemBlock::Resize(size_t iSize)
{
	if ( iSize <= TotalSpace() ) 
    {
		if ( _begin + iSize < _end ) 
        {
			_end = _begin + iSize;
		}		
		return 0;
	}

	MemBlock memBlockTmp(iSize, _allocator);
	if ( !memBlockTmp.GetBegin()) 
    {
		return -1;
	}
	
	if ( memBlockTmp.Append(_begin, ::std::min(Size(), iSize)) != 0 ) 
    {
		return -1;
	}
	
	this->Swap(memBlockTmp);

	return 0;
}


int MemBlock::Copy(const char* pDestBuffer, const char* pSrcBuffer, size_t iSize)
{
	ssize_t iDestOffset = pDestBuffer - _begin;

	if ( pDestBuffer < _begin ||  pDestBuffer > _end ) {
		return -1;
	}

	ssize_t iSrcOffset = pSrcBuffer - _begin;
	size_t iDestSize = iDestOffset + iSize;
	ssize_t iIncSize = iDestSize - Size();
	if ( iIncSize < 0 ) {
		iIncSize = 0;
	}

	if ( iDestSize > TotalSpace() ) 
    {
		if ( Resize(iDestSize) != 0 ) 
        {
			return -1;
		}
	}

	if ( iSrcOffset >= 0  && (size_t)iSrcOffset < Size() ) 
    {
		memmove(_begin+iDestOffset, _begin+iSrcOffset, iSize);
	}
	else 
    {		
		memcpy(_begin+iDestOffset, pSrcBuffer, iSize);
	}

	return AddSpace(iIncSize);
}

//int MemBlock::Erase(char* pBegin, char* pEnd)
//{
//	
//
//	if ( pBegin == pEnd ) {
//		return 0;
//	}
//
//	if ( pBegin > pEnd ) {
//		return -1;
//	}
//
//	if ( pBegin < _begin || pEnd > _end ) 
//    {
//		return -1;
//	}
//
//	if ( pBegin == _begin && pEnd == _end ) 
//    {
//		Clear();
//		return 0;
//	}
//
//	if ( Copy(pBegin, pEnd, _end-pEnd) != 0 ) 
//    {
//		return -1;
//	}
//
//	return DecEnd(pEnd-pBegin);
//}
	
int MemBlock::AddSpace(size_t iSize)
{
	if ( iSize > FreeSpace() ) 
    {		
		return -1;
	}
	_end += iSize;
	return 0;
}

int MemBlock::ReduceSpace(size_t iSize)
{
	if ( iSize > Size() ) 
    {
		return -1;
	}
	
	_end -= iSize;
	return 0;
}

}

