#include "FileMapping.h"

const DWORD FileMapping::ALLOCGRAN = FileMapping::GetAllocGran();

FileMapping::FileMapping(const TCHAR* filename, DWORD64 size)
{
	Create(filename, size);
}

FileMapping::FileMapping(DWORD64 size)
{
	Create(size);
}

FileMapping::FileMapping(HANDLE hFile, DWORD protect, DWORD64 size)
{
	Create(hFile, protect, size);
}

bool FileMapping::Create(const TCHAR* filename, DWORD64 size)
{
	closeHFile = true;
	hFile = CreateFile(filename, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
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
		CloseHandle(hMap);
	if (closeHFile && hFile)
		CloseHandle(hFile);
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