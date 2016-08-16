#include <tchar.h>
#include <Includes.h>
#include <File.h>
#include <Algorithm.h>
#include <ctime>
#include <GlobOps.h>

#ifdef _DEBUG
#define CONF "Debug"
#else
#define CONF "Release"
#endif
#if defined _X86_
#define ARCH "x32"
#elif defined _AMD64_
#define ARCH "x64"
#endif
#pragma comment(lib, ARCH"/" CONF"/TCPCS") 

#pragma comment(lib, "Algorithm")

void GenerateData(const wchar_t* filename, size_t dataSize)
{
	File file{ filename, GENERIC_WRITE, FILE_ATTRIBUTE_NORMAL, CREATE_ALWAYS };
	const size_t count = dataSize / sizeof(uint32_t);
	for (size_t i = 0; i < count; ++i)
	{
		uint32_t val = rand() % SIZE_MAX;
		file.Write(&val, sizeof(uint32_t));
	}	
}

int _tmain(int argc, TCHAR** argv)
{
	srand((uint32_t)time(NULL));
	FileMisc::SetCurDirectory(L"..\\Server");

	GenerateData(L"Data.dat", 10MB);
}