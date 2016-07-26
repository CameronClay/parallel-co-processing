#pragma once

//Fixes prob with min/max
#define NOMINMAX

#include <Includes.h>
#include <File.h>
#include <stdint.h>
#include <mutex>

class Data
{
public:
	Data();

	void LoadData(uint32_t expectedClientCount);
	uint32_t GetChunkSize() const;
	uint32_t GetClientWork(void* buffer, uint32_t buffSize, DWORD64& cursor);
	uint32_t GetClientWorkPrev(void* buffer, uint32_t buffSize, DWORD64 cursor);
private:
	File file;
	DWORD64 fileSize, cursorPos;
	uint32_t chunkSize;
};
