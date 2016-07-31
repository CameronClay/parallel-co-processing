#pragma once

#include <stdint.h>
#include <GlobOps.h>
#include <FileMapping.h>

class FileMappingChunkRW : public FileMapping
{
public:
	FileMappingChunkRW(const TCHAR* filename, DWORD64 size, const uint32_t chunkSize = 100MB)
		:
		FileMapping(filename, size),
		chunkMapSize(((chunkSize + FileMapping::ALLOCGRAN - 1) / FileMapping::ALLOCGRAN) * FileMapping::ALLOCGRAN) //Round to nearest FileMapping::ALLOCGRAN
	{}

	FileMappingChunkRW(const uint32_t chunkSize = 100MB)
		:
		FileMapping(),
		chunkMapSize(((chunkSize + FileMapping::ALLOCGRAN - 1) / FileMapping::ALLOCGRAN) * FileMapping::ALLOCGRAN) //Round to nearest FileMapping::ALLOCGRAN
	{}

	template<typename T>
	void Write(T* t, size_t size)
	{
		if (!writeBeg)
			RemapWrite();

		if (writeCur + size > writeEnd)
		{
			const size_t intersection = writeCur + size - writeEnd;
			memcpy(writeCur, t, intersection);

			RemapWrite();

			memcpy(writeCur, t, size - intersection);
		}
		else if (writeCur + size == writeEnd)
		{
			memcpy(writeCur, t, size);
			RemapWrite();
		}
		else
		{
			memcpy(writeCur, t, size);
			writeCur += size;
		}
	}

	template<typename T>
	void Read(T* t, size_t size)
	{
		if (!readBeg)
			RemapRead();

		if (readCur + size > readEnd)
		{
			const size_t intersection = readCur + size - readEnd;
			memcpy(t, readCur, intersection);

			RemapRead();

			memcpy(t, readCur, size - intersection);
		}
		else if (readCur + size == readEnd)
		{
			memcpy(t, readCur, size);
			RemapRead();
		}
		else
		{
			memcpy(t, readCur, size);
			readCur += size;
		}
	}

	template<typename T>
	void Read(T* t, uint64_t curPos, size_t size)
	{
		if (!readBeg)
			RemapRead();

		if (readPos >= curPos && (readCur - readEnd) + readPos > curPos + size)
		{
			memcpy(t, readBeg + (curPos - readPos), size);
		}
		else
		{
			char* temp = MapRandomAccess<char>(FILE_MAP_READ, curPos, size);
			memcpy(t, temp, size);
			Unmap(temp);
		}
	}

	void Write(FileMappingChunkRW& other, size_t curReadPos, size_t size)
	{
		if (!writeBeg)
			RemapWrite();

		if (writeCur + size > writeEnd)
		{
			const size_t intersection = writeCur + size - writeEnd;
			other.Read(writeCur, curReadPos, intersection);

			RemapWrite();

			other.Read(writeCur, curReadPos + intersection, size - intersection);
		}
		else if (writeCur + size == writeEnd)
		{
			other.Read(writeCur, curReadPos, size);
			RemapWrite();
		}
		else
		{
			other.Read(writeCur, curReadPos, size);
			writeCur += size;
		}
	}

	uint64_t GetWritePos() const
	{
		return writePos;
	}

	uint64_t GetReadPos() const
	{
		return writePos;
	}

protected:
	uint32_t chunkMapSize;

private:
	char *writeBeg = nullptr, *writeEnd = nullptr, *writeCur = nullptr,
		*readBeg = nullptr, *readEnd = nullptr, *readCur = nullptr;

	uint64_t writePos = 0, readPos = 0;

	void RemapWrite()
	{
		if(writeBeg)
			Unmap(writeBeg);
		writeCur = writeBeg = MapAllocGran<char>(FILE_MAP_WRITE, writePos, chunkMapSize);
		writeEnd = writeBeg + chunkMapSize;
		writePos += chunkMapSize;
	}

	void RemapRead()
	{
		if(readBeg)
			Unmap(readBeg);
		readCur = readBeg = MapAllocGran<char>(FILE_MAP_READ, readPos, chunkMapSize);
		readEnd = readBeg + chunkMapSize;
		readCur += chunkMapSize;
	}
};