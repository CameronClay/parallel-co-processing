#include "Data.h"


Data::Data()
	:
	fileSize(0),
	chunkSize(0),
	cursorPos(0),
	curIndex(0)
{}

void Data::LoadData(uint32_t expectedClientCount)
{
	file.Open(L"Data.dat", GENERIC_READ);
	fileSize = file.GetSize();
	chunkSize = fileSize / expectedClientCount;
	cursorPos = 0;
}

uint32_t Data::GetChunkSize() const
{
	return chunkSize;
}

WorkInfo Data::GetClientWork(void* buffer, uint32_t buffSize)
{
	if (buffSize < chunkSize)
		return{};

	WorkInfo wi;

	wi.cursor = cursorPos;
	wi.curIndex = curIndex++;
	const uint32_t read = wi.size = file.Read(buffer, buffSize);
	cursorPos += read;
	return wi;
}

WorkInfo Data::GetClientWorkPrev(void* buffer, uint32_t buffSize, const WorkInfo& wi)
{
	file.MoveCursor(cursorPos - wi.cursor);
	const uint32_t read = file.Read(buffer, buffSize);
	file.MoveCursor(wi.cursor - cursorPos);
	return wi;
}

DWORD64 Data::GetFileSize() const
{
	return fileSize;
}