#pragma once
#include <stdint.h>
#include <FileMappingChunkRW.h>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <GlobOps.h>
#include <Includes.h>
#include <File.h>

class FileMappingIndexed : public FileMappingChunkRW
{
public:
	FileMappingIndexed(const TCHAR* filename, DWORD64 size, const size_t chunkSize = 100MB)
		:
		FileMappingChunkRW(filename, size, chunkSize)
	{}
	FileMappingIndexed(const size_t chunkSize = 100MB)
		:
		FileMappingChunkRW(chunkSize)
	{}

	template<typename T>
	void Write(uint32_t index, T* t, size_t size)
	{
		auto it = std::upper_bound(indices.begin(), indices.end(), index, &IndexEntry::Less);
		indices.emplace(it, index, GetWritePos(), size);

		__super::Write(t, size);
	}

	void ReorderFileData()
	{
		uint64_t curPos = GetWritePos();
		FileMappingChunkRW mp{ _T("TempData.dat"), curPos };
		for (auto it = indices.begin(), end = indices.end(); it != end; ++it)
		{
			mp.Write(*this, it->pos, it->size);
		}
		mp.Close();
		Close();

		FileMisc::Remove(L"NewData.dat");
		FileMisc::MoveOrRename(L"TempData.dat", L"NewData.dat");
	}
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

		static inline bool Less(uint32_t index, const IndexEntry& rhs)
		{
			return index < rhs.index;
		}

		uint32_t index = 0;
		uint64_t pos = 0;
		size_t size = 0;
	};

	std::list<IndexEntry> indices;
};
