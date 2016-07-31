#include <DataInterp.h>
#include "Data.h"

#ifdef _X86_
#pragma comment(lib, "x32/TCPCS")
#else
#pragma comment(lib, "x64/TCPCS")
#endif

Data data;

const SERVDLL float DataInterp::ACCEPTEDTIME = 750.0f;
const SERVDLL float DataInterp::MAXTIME = DataInterp::ACCEPTEDTIME * DataInterp::ACCEPTEDTIME;

void DataInterp::LoadData(uint32_t expectedClientCount)
{
	data.LoadData(expectedClientCount);
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