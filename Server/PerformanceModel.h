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

	PerformanceModel(uint32_t nStandardDeviations = 3);

	Status EvaluateTime(float time);
private:
	StandardDeviation sd;
	uint32_t nStandardDeviations;
};