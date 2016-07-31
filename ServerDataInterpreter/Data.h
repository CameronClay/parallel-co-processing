#pragma once

//Fixes prob with min/max
#define NOMINMAX

#include <Includes.h>
#include <File.h>
#include <stdint.h>
#include <mutex>
#include <DataInterp.h>

class Data
{
public:
	Data();

	void LoadData(uint32_t expectedClientCount);
	uint32_t GetChunkSize() const;
	WorkInfo GetClientWork(void* buffer, uint32_t buffSize);
	WorkInfo GetClientWorkPrev(void* buffer, uint32_t buffSize, const WorkInfo& wi);
	DWORD64 GetFileSize() const;
private:
	File file;
	DWORD64 fileSize, cursorPos;
	uint32_t chunkSize, curIndex;
};
