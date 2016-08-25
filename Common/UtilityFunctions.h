#pragma once
#include <type_traits>
#include <stdint.h>

namespace UtilityFunctions
{
	template<typename T> 
	constexpr std::enable_if_t<std::is_unsigned<T>::value, T> RoundToMultiple(const T& t, const T& multiple)
	{
		static_assert(std::is_integral<T>::value, "Cannot round non integral type to nearest multiple");

		return ((t + multiple - 1) / multiple) * multiple;
	}

	template<typename T>
	constexpr std::enable_if_t<std::is_signed<T>::value, T> RoundToMultiple(const T& t, const T& multiple)
	{
		static_assert(std::is_integral<T>::value, "Cannot round non integral type to nearest multiple");

		return ((t + (T)(t >= 0) * (multiple - 1)) / multiple) * multiple;
	}

	template<typename T>
	constexpr bool InRangeInc(const T& val, const T& r1, const T& r2)
	{
		return (val <= r2) && (val >= r1);
	}

	template<typename T>
	constexpr bool InRangeExc(const T& val, const T& r1, const T& r2)
	{
		return (val < r2) && (val > r1);
	}
}
