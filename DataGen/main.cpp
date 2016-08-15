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

bool VerifyNewData(const wchar_t* newFilename, const wchar_t* oldFilename)
{
	static const uint32_t buffSize = 4096;
	const uint32_t tempSize = Algorithm::GetOutSize(buffSize);
	auto newBuff = std::make_unique<char[]>(buffSize), oldBuff = std::make_unique<char[]>(buffSize), tempBuff = std::make_unique<char[]>(tempSize);
	File newData{ newFilename, GENERIC_READ }, oldData{ oldFilename, GENERIC_READ };
	DWORD read = 0;
	uint32_t size = 0;
	while (true)
	{
		bool eof = false;
		read = oldData.Read(oldBuff.get(), buffSize);
		if (!read)
			eof = true;
		else
			size = Algorithm::AlgorithmInOut(oldBuff.get(), buffSize, tempBuff.get(), tempSize);

		read = newData.Read(newBuff.get(), size ? size : 1);
		if (!read)
		{
			if (eof)
				return true;
			else 
				return false;
		}

		if (memcmp(newBuff.get(), tempBuff.get(), size) != 0)
			return false;
	}
}

int _tmain(int argc, TCHAR** argv)
{
	srand(time(NULL));
	FileMisc::SetCurDirectory(L"..\\Server");
	//GenerateData(L"Data.dat", 1MB);
	bool verified = VerifyNewData(L"NewData.dat", L"Data.dat");
	_tprintf(_T("Verified: %d\n"), verified);
	TCHAR buffer[64];
	_getts_s(buffer);
}