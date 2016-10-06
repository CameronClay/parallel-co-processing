#pragma once
#include <stdint.h>
#include <FileMappingChunkRW.h>
#include <set>
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
		auto it = std::upper_bound(indices.begin(), indices.end(), index, [](uint32_t index, const IndexEntry& rhs) 
		{
			return index < rhs.index;
		});

		indices.emplace(it->index, GetWritePos(), size);

		__super::Write(t, size);
	}

	void ReorderFileData()
	{
		const uint64_t curPos = GetWritePos();
		FileMappingChunkRW mp{ _T("TempData.dat"), curPos, CREATE_ALWAYS };

		for (auto& it : indices)
			mp.Write(*this, it.pos, it.size);

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

		bool operator<(IndexEntry& entry) const noexcept
		{
			return index < entry.index;
		}

		uint32_t index = 0;
		uint64_t pos = 0;
		size_t size = 0;
	};
	struct Compare
	{
		bool operator()(const IndexEntry& lhs, const IndexEntry& rhs) const noexcept
		{
			return lhs.index < rhs.index;
		}
	};

	std::set<IndexEntry, Compare> indices;
};
