#pragma once
#include <Windows.h>

class FileMapping
{
public:
	static const DWORD ALLOCGRAN;

	FileMapping() = default;
	FileMapping(const TCHAR* filename, DWORD64 size);
	FileMapping(DWORD64 size); 	//Backed by system page instead of file
	FileMapping(HANDLE hFile, DWORD protect, DWORD64 size);
	~FileMapping();

	bool Create(const TCHAR* filename, DWORD64 size);
	bool Create(DWORD64 size); 	//Backed by system page instead of file
	bool Create(HANDLE hFile, DWORD protect, DWORD64 size);

	void Close();

	HANDLE GetHFile() const;

	//desiredAccess = FILE_MAP_WRITE, FILE_MAP_READ, FILE_MAP_COPY, FILE_MAP_ALL_ACCESS
	template<typename T>
	T* MapAllocGran(DWORD desiredAccess, DWORD64 pos, size_t size)
	{
		return (T*)MapViewOfFile(hMap, desiredAccess, pos >> 32, pos & MAXDWORD, size);
	}

	//desiredAccess = FILE_MAP_WRITE, FILE_MAP_READ, FILE_MAP_COPY, FILE_MAP_ALL_ACCESS
	template<typename T>
	T* MapRandomAccess(DWORD desiredAccess, DWORD64 pos, size_t size)
	{
		const DWORD offset = pos % ALLOCGRAN;
		pos -= offset;
		return MapAllocGran<T>(desiredAccess, pos, size + offset) + offset;
	}

	template<typename T>
	void Unmap(T* t)
	{
		UnmapViewOfFile(t);
	}

	template<typename T>
	void Flush(T* t, size_t size)
	{
		FlushViewOfFile(t, size);
	}

	static DWORD GetAllocGran();
private:
	HANDLE hFile = NULL, hMap = NULL;
	bool closeHFile = false;
};