#include "Data.h"

Data::Data()
	:
	fileSize(std::numeric_limits<size_t>::max()),
	chunkSize(std::numeric_limits<size_t>::max()),
	cursorPos(0)
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

uint32_t Data::GetClientWork(void* buffer, uint32_t buffSize, DWORD64& cursor)
{
	if (buffSize < chunkSize)
		return 0;

	cursor = cursorPos;
	const uint32_t read = file.Read(buffer, buffSize);
	cursorPos += read;
	return read;
}

uint32_t Data::GetClientWorkPrev(void* buffer, uint32_t buffSize, DWORD64 cursor)
{
	file.MoveCursor(cursorPos - cursor);
	const uint32_t read = file.Read(buffer, buffSize);
	file.MoveCursor(cursor - cursorPos);
	return read;
}