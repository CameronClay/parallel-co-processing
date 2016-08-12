#include <Algorithm.h>

uint32_t Algorithm::GetOutSize(uint32_t inSize)
{
	return inSize;
}
uint32_t Algorithm::AlgorithmInOut(void* in, uint32_t inSize, void* out, uint32_t outSize)
{
	if (!in || !out || outSize < GetOutSize(inSize))
		return 0;

	inSize /= sizeof(uint32_t);
	for (uint32_t *pin = (uint32_t*)in, *pout = (uint32_t*)out, *pend = ((uint32_t*)in) + inSize; pin != pend; ++pin)
	{
		*pout = *pin * *pin;
	}

	return inSize;
}