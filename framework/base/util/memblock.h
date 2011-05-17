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
	MemBlock(const MemBlock &);
	~MemBlock();

	void Swap(MemBlock &);
	int Resize(size_t iSize);
	inline void Clear() { _end = _begin; }


	int IncEnd(size_t iSize);
	int DecEnd(size_t iSize);

	int Copy(char* pDestBuffer, const char* pSrcBuffer, size_t iSize);
	inline int Append(const char* pSrcBuffer, size_t iSize) { return Copy(GetEnd(), pSrcBuffer, iSize); }
	int Erase(char* pBegin, char* pEnd);


	inline char* GetBegin() { return _begin; }
	inline char* GetEnd() { return _end; }
	inline const char* GetBegin() const { return _begin; }
	inline const char* GetEnd() const { return _end; }
	inline bool IsInit() const { return GetBegin() != NULL; }
	inline bool IsEmpty() const { return GetBegin() == GetEnd(); }
	inline bool IsFull() const { return GetEnd() == _blockEnd; }
	inline size_t GetSize() const  { return GetEnd() - GetBegin(); }
	inline size_t GetLength() const  { return GetSize(); }
	inline size_t GetAvailable() const { return _blockEnd - GetEnd(); }
	inline size_t GetCapacity() const  { return _blockEnd - GetBegin(); }


private:
	IMemoryAllocator * _allocator;
	char* _begin;
	char* _end;
	char* _blockEnd;
};

}

#endif

