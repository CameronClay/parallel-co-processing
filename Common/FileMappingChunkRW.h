#pragma once
#include <stdint.h>
#include <GlobOps.h>
#include <FileMapping.h>
#include <UtilityFunctions.h>

class FileMappingChunkRW : public FileMapping
{
public:
	FileMappingChunkRW(const TCHAR* filename, DWORD64 size, DWORD createFlags = OPEN_ALWAYS, const size_t chunkSize = 100MB)
		:
		FileMapping(filename, size, createFlags),
		chunkMapSize(min(size, UtilityFunctions::RoundToMultiple<size_t>(chunkSize, FileMapping::ALLOCGRAN))) //Round to nearest FileMapping::ALLOCGRAN
	{}

	FileMappingChunkRW(const size_t chunkSize = 100MB)
		:
		FileMapping(),
		chunkMapSize(UtilityFunctions::RoundToMultiple<size_t>(chunkSize, FileMapping::ALLOCGRAN)) //Round to nearest FileMapping::ALLOCGRAN
	{}

	~FileMappingChunkRW()
	{
		Close();
	}

	void Close()
	{
		if (writeBeg)
			Unmap(writeBeg);
		if (readBeg)
			Unmap(readBeg);

		__super::Close();
	}

	bool Create(const TCHAR* filename, DWORD64 size, DWORD createFlags = OPEN_ALWAYS)
	{
		chunkMapSize = min(chunkMapSize, size);
		return __super::Create(filename, size, createFlags);
	}
	bool Create(DWORD64 size) 	//Backed by system page instead of file
	{
		chunkMapSize = min(chunkMapSize, size);
		return __super::Create(size);
	}
	bool Create(HANDLE hFile, DWORD protect, DWORD64 size)
	{
		chunkMapSize = min(chunkMapSize, size);
		return __super::Create(hFile, protect, size);
	}

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
				const uint64_t startReadPos = GetReadPos(), endReadPos = startReadPos + (readEnd - readBeg);
				if (curPos + size > endReadPos)
				{
					const size_t intersection = (curPos + size) - endReadPos;
					memcpy(t, readBeg + (curPos - readPos), intersection);

					t += intersection;
					size -= intersection;
					RemapRead();
				}
				else if (curPos + size == endReadPos)
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
				char* baseAddress = nullptr;
				char* temp = MapRandomAccess<char>(baseAddress, FILE_MAP_READ, curPos, size);
				memcpy(t, temp, size);
				Unmap(baseAddress);

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

	uint64_t GetMappedWritePos() const
	{
		return writePos;
	}

	uint64_t GetMappedReadPos() const
	{
		return readPos;
	}

	uint64_t GetWritePos() const
	{
		return writePos + (writeCur - writeBeg);
	}

	uint64_t GetReadPos() const
	{
		return readPos + (readCur - readBeg);
	}

protected:
	size_t chunkMapSize;

	char *writeBeg = nullptr, *writeEnd = nullptr, *writeCur = nullptr,
		*readBeg = nullptr, *readEnd = nullptr, *readCur = nullptr;

	uint64_t writePos = 0, readPos = 0;

private:
	void RemapWrite()
	{
		if (writeBeg)
		{
			Unmap(writeBeg);
			writeCur = writeBeg = MapAllocGran<char>(FILE_MAP_WRITE, writePos, chunkMapSize);
			writePos += chunkMapSize;
		}
		else
		{
			writeCur = writeBeg = MapAllocGran<char>(FILE_MAP_WRITE, writePos, chunkMapSize);
		}

		writeEnd = writeBeg + chunkMapSize;
	}

	void RemapRead()
	{
		if (readBeg)
		{
			Unmap(readBeg);
			readCur = readBeg = MapAllocGran<char>(FILE_MAP_READ, readPos, chunkMapSize);
			readPos += chunkMapSize;
		}
		else
		{
			readCur = readBeg = MapAllocGran<char>(FILE_MAP_READ, readPos, chunkMapSize);
		}

		readEnd = readBeg + chunkMapSize;
	}
};