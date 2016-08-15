#include "Data.h"

Data::Data()
	:
	fileSize(0),
	chunkSize(0),
	cursorPos(0),
	curIndex(0)
{}

void Data::LoadData(uint32_t expectedClientCount, uint32_t desiredChunkSize)
{
	file.Open(L"Data.dat", GENERIC_READ);
	fileSize = file.GetSize();

	if (fileSize > desiredChunkSize)
		chunkSize = desiredChunkSize;
	else
		chunkSize = fileSize;

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

	const uint32_t read = file.Read(buffer, buffSize);
	if (read)
	{
		cursorPos += read;
		return{ cursorPos, read, curIndex++ };
	}
	return {};
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