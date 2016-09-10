#pragma once

#include "StandardDeviation.h"

class PerformanceModel
{
public:
	enum Status
	{
		FAST,
		NORMAL,
		TOOSLOW
	};

	PerformanceModel(uint32_t nSDHigh = 2, uint32_t nSDLow = 4);

	Status EvaluateTime(float time);
private:
	StandardDeviation sd;
	uint32_t nSDHigh, nSDLow;
};