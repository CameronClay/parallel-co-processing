#pragma once
#include <stdint.h>

namespace
{
	constexpr uint64_t KB(const uint32_t n)
	{
		return n * 1024;
	}

	constexpr uint64_t MB(const uint32_t n)
	{
		return n * KB(1024);
	}

	constexpr uint64_t GB(const uint32_t n)
	{
		return n * MB(1024);
	}
}