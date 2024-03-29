#pragma once
#include <cstdint>

#ifdef DLLEXPORT
#define SERVDLL __declspec(dllexport)
#else
#define SERVDLL __declspec(dllimport)
#endif

struct WorkInfo
{
	WorkInfo() = default;
	WorkInfo(uint64_t cursor, uint32_t size, uint32_t curIndex)
		:
		size(size),
		curIndex(curIndex),
		cursor(cursor)
	{}

	uint64_t cursor = 0;
	uint32_t size = 0, curIndex = 0;
};

//All functions here are non-thread-safe
namespace DataInterp
{
	SERVDLL void LoadData(uint32_t expectedClientCount, uint32_t desiredChunkSize);
	SERVDLL unsigned long long GetFileSize();
	SERVDLL uint32_t GetChunkSize();
	SERVDLL WorkInfo GetClientWork(void* out, uint32_t buffSize);
	SERVDLL WorkInfo GetClientWorkPrev(void* out, uint32_t buffSize, const WorkInfo& wi);

	extern SERVDLL const uint32_t WAITTIME;
	extern SERVDLL const uint32_t BUFFERSIZE;
	extern SERVDLL const bool ORDERED;
};
