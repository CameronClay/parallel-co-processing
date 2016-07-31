#pragma once

#pragma warning( disable : 4455 )  // disable "literals not starting with _ are reserved"

constexpr unsigned long long operator ""KB(unsigned long long n)
{
	return n * 1024;
}

constexpr unsigned long long operator ""MB(unsigned long long n)
{
	return n * 1024KB;
}

constexpr unsigned long long operator ""GB(unsigned long long n)
{
	return n * 1024MB;
}

constexpr unsigned long long operator ""TB(unsigned long long n)
{
	return n * 1024GB;
}

constexpr unsigned long long operator ""PB(unsigned long long n)
{
	return n * 1024TB;
}

constexpr unsigned long long operator ""EB(unsigned long long n)
{
	return n * 1024PB;
}
