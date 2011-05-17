#ifndef _BASE_MEMBLOCK_H_
#define _BASE_MEMBLOCK_H_

#include <sys/types.h>
#include <base/util/allocator.h>


namespace base
{

class MemBlock
{
public:
	const static int DEFAULT_SIZE = 1024;

public:

	MemBlock(size_t iSize = DEFAULT_SIZE, IMemoryAllocator *pAllocator = GxxMemoryAllocator::Instance());
	MemBlock(const MemBlock & memblock);
	~MemBlock();

	void Swap(MemBlock & memblock);
	int Resize(size_t iSize);
	void Clear() { _end = _begin; }

	int Copy(const char* pDestBuffer, const char* pSrcBuffer, size_t iSize);
	int Append(const char* pSrcBuffer, size_t iSize) { return Copy(_end, pSrcBuffer, iSize); }
    int AddSpace(size_t iSize);
    int ReduceSpace(size_t iSize);

	char* GetBegin() { return _begin; }
	const char* GetBegin() const { return _begin; }
	const char* GetEnd() const { return _end; }
	bool Empty() const { return _begin == _end; }
	bool Full() const { return _end == _blockEnd; }
	size_t Size() const  { return _end - _begin; }

protected:
	size_t FreeSpace() const { return _blockEnd - GetEnd(); }
	size_t TotalSpace() const  { return _blockEnd - GetBegin(); }


private:
	IMemoryAllocator * _allocator;
	char* _begin;
	char* _end;
	char* _blockEnd;
};

}

#endif

