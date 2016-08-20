#pragma once
#include <Includes.h>
#include <BuffAllocator.h>
#include <MemPool.h>
#include <HeapAlloc.h>

class PoolAlloc : public BuffAllocator
{
public:
	PoolAlloc(DWORD maxElementSize, size_t initialSize);

	char* alloc(DWORD nBytes) override;
	void dealloc(char* data, DWORD nBytes = 0) override;
private:
	MemPoolSync<PageAllignAllocator> memPool;
};