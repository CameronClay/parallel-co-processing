#include "FileMapping.h"

const DWORD FileMapping::ALLOCGRAN = FileMapping::GetAllocGran();

FileMapping::FileMapping(const TCHAR* filename, DWORD64 size, DWORD createFlags)
{
	Create(filename, size, createFlags);
}

FileMapping::FileMapping(DWORD64 size)
{
	Create(size);
}

FileMapping::FileMapping(HANDLE hFile, DWORD protect, DWORD64 size)
{
	Create(hFile, protect, size);
}

bool FileMapping::Create(const TCHAR* filename, DWORD64 size, DWORD createFlags)
{
	closeHFile = true;
	hFile = CreateFile(filename, GENERIC_READ | GENERIC_WRITE, 0, NULL, createFlags, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		hFile = NULL;
		return false;
	}

	return hMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE, size >> 32, size & MAXDWORD, NULL);
}

bool FileMapping::Create(DWORD64 size)
{
	closeHFile = true;
	return hMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, size >> 32, size & MAXDWORD, NULL);
}

bool FileMapping::Create(HANDLE hFile, DWORD protect, DWORD64 size)
{
	closeHFile = false;
	this->hFile = hFile;
	return hMap = CreateFileMapping(hFile, NULL, protect, size >> 32, size & MAXDWORD, NULL);
}

void FileMapping::Close()
{
	if (hMap)
	{
		CloseHandle(hMap);
		hMap = NULL;
	}
	if (closeHFile && hFile)
	{
		CloseHandle(hFile);
		hFile = NULL;
	}
}

HANDLE FileMapping::GetHFile() const
{
	return hFile;
}

FileMapping::~FileMapping()
{
	Close();
}

DWORD FileMapping::GetAllocGran()
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	return si.dwAllocationGranularity;
}