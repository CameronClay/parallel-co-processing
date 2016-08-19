#pragma once
#include <type_traits>
#include <stdint.h>

#define IntTypeBits(size)\
intsize_t

#define IntTypeBytes(size)\
IntTypeBits(size * 8)

namespace UtilityFunctions
{
	template<typename T> 
	std::enable_if_t<std::is_unsigned<T>::value, T> RoundToMultiple(const T& t, const T& multiple)
	{
		static_assert(std::is_integral<T>::value, "Cannot round non integral type to nearest multiple");

		return ((t + multiple - 1) / multiple) * multiple;
	}

	template<typename T>
	std::enable_if_t<std::is_signed<T>::value, T> RoundToMultiple(const T& t, const T& multiple)
	{
		static_assert(std::is_integral<T>::value, "Cannot round non integral type to nearest multiple");

		return ((numToRound + (T)(numToRound >= 0) * (multiple - 1)) / multiple) * multiple;
	}
}
