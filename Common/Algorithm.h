#pragma once
#include <stdint.h>

#ifdef DLLEXPORT
#define CLNTALGORITHM __declspec(dllexport)
#else
#define CLNTALGORITHM __declspec(dllimport)
#endif

namespace Algorithm
{
	CLNTALGORITHM uint32_t GetOutSize(uint32_t inSize);
	CLNTALGORITHM void AlgorithmInOut(void* in, uint32_t inSize, void* out, uint32_t outSize);
	CLNTALGORITHM void AlgorithmIn(void* in, uint32_t inSize);
}