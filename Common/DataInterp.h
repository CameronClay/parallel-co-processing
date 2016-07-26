#pragma once
#include <cstdint>

#ifdef DLLEXPORT
#define SERVDLL __declspec(dllexport)
#else
#define SERVDLL __declspec(dllimport)
#endif

//All functions here are non-thread-safe
namespace DataInterp
{
	SERVDLL void LoadData(uint32_t expectedClientCount);
	SERVDLL uint32_t GetChunkSize();
	SERVDLL uint32_t GetClientWork(void* out, uint32_t buffSize, unsigned long long& cursor);
	SERVDLL uint32_t GetClientWorkPrev(void* out, uint32_t buffSize, unsigned long long cursor);

	extern const SERVDLL float ACCEPTEDTIME;
	extern const SERVDLL float MAXTIME;
};
