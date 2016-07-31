#pragma once
#include <stdint.h>

#ifdef DLLEXPORT
#define CLNTALGORITHIM __declspec(dllexport)
#else
#define CLNTALGORITHIM __declspec(dllimport)
#endif

namespace Algorithim
{
	CLNTALGORITHIM uint32_t GetOutSize(uint32_t inSize);
	CLNTALGORITHIM void AlgorithmInOut(void* in, uint32_t inSize, void* out, uint32_t outSize);
	CLNTALGORITHIM void AlgorithmIn(void* in, uint32_t inSize);
}