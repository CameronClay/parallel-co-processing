#pragma once
#include <stdint.h>
#include <FileMappingChunkRW.h>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <GlobOps.h>

class FileMappingIndexed : public FileMappingChunkRW
{
public:
	FileMappingIndexed(const TCHAR* filename, DWORD64 size, const uint32_t chunkSize = 100MB)
		:
		FileMappingChunkRW(filename, size, chunkSize)
	{}
	FileMappingIndexed(const uint32_t chunkSize = 100MB)
		:
		FileMappingChunkRW(chunkSize)
	{}

	void CopyDataToFile()
	{
		char* p = nullptr;
		uint64_t curPos = GetWritePos();
		FileMappingChunkRW mp{ _T("NewData.dat"), curPos };
		for (auto it = indices.begin(), end = indices.end(); it != end; ++it)
		{
			mp.Write(*this, it->pos, it->size);
		}
		//for (size_t pos = 0, endPos = chunkMapSize; pos < curPos; pos = endPos, endPos += chunkMapSize)
		//{
		//	if (indices.empty())
		//		return;

		//	//Flush(p, chunkMapSize);
		//	for (auto it = indices.begin(), end = indices.end(); it != end; ++it)
		//	{
		//		//need to handle case where pos isnt in chunk...
		//		if (it->pos >= pos && it->pos + it->size < endPos)
		//		{
		//			//write to file at p + pos - it->pos

		//			mp.Write(*this, it->pos,  it->size);
		//			memcpy(curP, p + pos - it->pos, it->size);
		//			it = indices.erase(it);
		//		}
		//		else
		//		{
		//			//LONG moveHigh = pos >> 32;
		//			//SetFilePointer(hFile, pos & MAXDWORD, &moveHigh, FILE_BEGIN);
		//			//char* buffer = new char[it->size];
		//			//DWORD read = 0;
		//			//ReadFile(hFile, buffer, it->size, &read, NULL);

		//			////write to file
		//			//delete[] buffer;
		//		}
		//	}
		//}
	}
	//uint32_t lastIndex = -1;
	//for (auto it = indices.begin(), end = indices.end(); it != end; ++it)
	//{
	//	if (it->index != lastIndex + 1)
	//	{
	//		//const size_t size = it->curPos + it->size - indices.front().curPos;
	//		//if ((it != indices.begin()) && (size > chunkMapSize))
	//		//{
	//		//	char* c  = MapRandomAccess<char>(FILE_MAP_READ, indices.front().curPos, size);
	//		//	file.Write(c, size);
	//		//	Unmap(c);
	//		//	//erase all entries up to this point and copy data to actual file
	//		//	indices.erase(indices.begin(), it);
	//		//}
	//		return;
	//	}
	//}
private:
	struct IndexEntry
	{
		IndexEntry() = default;
		IndexEntry(uint32_t index, uint64_t pos, size_t size)
			:
			index(index),
			pos(pos),
			size(size)
		{}

		static inline bool Less(const IndexEntry& lhs, const IndexEntry& rhs)
		{
			return lhs.index < rhs.index;
		}

		uint32_t index = 0;
		uint64_t pos = 0;
		size_t size = 0;
	};

	std::list<IndexEntry> indices;
};
