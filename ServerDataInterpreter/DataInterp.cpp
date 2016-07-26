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

uint32_t DataInterp::GetChunkSize()
{
	return data.GetChunkSize();
}

uint32_t DataInterp::GetClientWork(void* out, uint32_t buffSize, unsigned long long& cursor)
{
	return data.GetClientWork(out, buffSize, cursor);
}

uint32_t DataInterp::GetClientWorkPrev(void* out, uint32_t buffSize, unsigned long long cursor)
{
	return data.GetClientWorkPrev(out, buffSize, cursor);
}