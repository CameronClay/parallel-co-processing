#include "PerformanceModel.h"

PerformanceModel::PerformanceModel(uint32_t nStandardDeviations)
	:
	nStandardDeviations(nStandardDeviations)
{}

auto PerformanceModel::EvaluateTime(float time) -> Status
{
	auto data = sd.Calculate(time);
	if (data.mean == 0)
		return NORMAL;

	const double diff = data.sd * nStandardDeviations;
	if (time <= data.mean - diff)
		return FAST;
	else if (time > data.mean + diff)
		return TOOSLOW;

	return NORMAL;
}