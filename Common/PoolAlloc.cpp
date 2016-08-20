#include "PoolAlloc.h"

PoolAlloc::PoolAlloc(DWORD maxElementSize, size_t initialSize)
	:
	memPool(maxElementSize, initialSize)
{}

char* PoolAlloc::alloc(DWORD nBytes)
{
	return static_cast<char*>(memPool.alloc(nBytes));
}
void PoolAlloc::dealloc(char* data, DWORD nBytes)
{
	memPool.dealloc(data);
}