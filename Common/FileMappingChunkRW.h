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

		while (size)
		{
			if (writeCur + size > writeEnd)
			{
				const size_t intersection = writeCur + size - writeEnd;
				memcpy(writeCur, t, intersection);

				writeCur += intersection;
				t += intersection;
				size -= intersection;

				RemapWrite();

				//memcpy(writeCur, t + intersection, size - intersection);
				//writeCur += size - intersection;
			}
			else if (writeCur + size == writeEnd)
			{
				memcpy(writeCur, t, size);
				RemapWrite();

				size = 0;
			}
			else
			{
				memcpy(writeCur, t, size);

				writeCur += size;
				size = 0;
			}
		}
	}

	template<typename T>
	void Read(T* t, size_t size)
	{
		if (!readBeg)
			RemapRead();

		while (size)
		{
			if (readCur + size > readEnd)
			{
				const size_t intersection = readCur + size - readEnd;
				memcpy(t, readCur, intersection);

				readCur += intesection;
				t += intersection;
				size -= intersection;

				RemapRead();
			}
			else if (readCur + size == readEnd)
			{
				memcpy(t, readCur, size);
				RemapRead();

				size = 0;
			}
			else
			{
				memcpy(t, readCur, size);

				readCur += size;
				size = 0;
			}
		}
	}

	template<typename T>
	void Read(T* t, uint64_t curPos, size_t size)
	{
		if (!readBeg)
			RemapRead();

		while (size)
		{
			if (readPos >= curPos)
			{
				if (curPos + size > (readCur - readEnd) + readPos)
				{
					const size_t intersection = ((readCur - readEnd) + readPos) - (curPos + size);
					memcpy(t, readBeg + (curPos - readPos), intersection);

					t += intersection;
					size -= intersection;
					RemapRead();
				}
				else if (curPos + size == (readCur - readEnd) + readPos)
				{
					memcpy(t, readBeg + (curPos - readPos), size);
					RemapRead();

					size = 0;
				}
				else
				{
					memcpy(t, readBeg + (curPos - readPos), size);

					size = 0;
				}
			}
			else
			{
				char* temp = MapRandomAccess<char>(FILE_MAP_READ, curPos, size);
				memcpy(t, temp, size);
				Unmap(temp);

				size = 0;
			}
		}
	}

	void Write(FileMappingChunkRW& other, size_t curReadPos, size_t size)
	{
		if (!writeBeg)
			RemapWrite();

		while (size)
		{
			if (writeCur + size > writeEnd)
			{
				const size_t intersection = writeCur + size - writeEnd;
				other.Read(writeCur, curReadPos, intersection);

				writeCur += intersection;
				curReadPos += intersection;
				size -= intersection;

				RemapWrite();
			}
			else if (writeCur + size == writeEnd)
			{
				other.Read(writeCur, curReadPos, size);
				RemapWrite();

				size = 0;
			}
			else
			{
				other.Read(writeCur, curReadPos, size);

				writeCur += size;
				size = 0;
			}
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