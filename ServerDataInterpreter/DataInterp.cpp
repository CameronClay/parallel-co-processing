#include <stdint.h>

__declspec(dllexport) const float ACCEPTEDTIME = 500.0f;

extern "C" __declspec(dllexport) void LoadData()
{

}

extern "C" __declspec(dllexport) void BeginProcess(uint32_t expectedClientCount)
{

}

extern "C" __declspec(dllexport) uint32_t GetClientWork(void* out)
{
	return 0;
}