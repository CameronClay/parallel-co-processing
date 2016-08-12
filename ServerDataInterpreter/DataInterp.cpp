#include <DataInterp.h>
#include "Data.h"

#ifdef _X86_
#ifdef _DEBUG
#pragma comment(lib, "x32/Debug/TCPCS")
#else
#pragma comment(lib, "x32/Release/TCPCS")
#endif
#elif defined _AMD64_
#ifdef _DEBUG
#pragma comment(lib, "x64/Debug/TCPCS")
#else
#pragma comment(lib, "x64/Release/TCPCS")
#endif
#endif

Data data;

const SERVDLL float DataInterp::ACCEPTEDTIME = 750.0f;
const SERVDLL float DataInterp::MAXTIME = DataInterp::ACCEPTEDTIME * DataInterp::ACCEPTEDTIME;

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