#pragma once
#include <cstdint>

#ifdef DLLEXPORT
#define SERVDLL __declspec(dllexport)
#else
#define SERVDLL __declspec(dllimport)
#endif

SERVDLL void LoadDataProc(uint32_t expectedClientCount);
SERVDLL uint32_t GetClientWorkProc(void* out, uint32_t buffSize);

SERVDLL extern const float ACCEPTEDTIME;
SERVDLL extern const float MAXTIME;