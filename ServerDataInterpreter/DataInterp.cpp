#include <DataInterp.h>
#include "Data.h"

#ifdef _DEBUG
#define CONF "Debug"
#else
#define CONF "Release"
#endif
#if defined _X86_
#define ARCH "x32"
#elif defined _AMD64_
#define ARCH "x64"
#endif
#pragma comment(lib, ARCH"/" CONF"/TCPCS") 

Data data;

SERVDLL const uint32_t DataInterp::WAITTIME = 15;
SERVDLL const bool DataInterp::ORDERED = true;

void DataInterp::LoadData(uint32_t expectedClientCount, uint32_t desiredChunkSize)
{
	data.LoadData(expectedClientCount, desiredChunkSize);
}

unsigned long long DataInterp::GetFileSize()
{
	return data.GetFileSize();
}

uint32_t DataInterp::GetChunkSize()
{
	return data.GetChunkSize();
}

WorkInfo DataInterp::GetClientWork(void* out, uint32_t buffSize)
{
	return data.GetClientWork(out, buffSize);
}

WorkInfo DataInterp::GetClientWorkPrev(void* out, uint32_t buffSize, const WorkInfo& wi)
{
	return data.GetClientWorkPrev(out, buffSize, wi);
}