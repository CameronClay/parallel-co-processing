#include "PerformanceModel.h"

PerformanceModel::PerformanceModel(uint32_t nSDHigh, uint32_t nSDLow)
	:
	nSDHigh(nSDHigh),
	nSDLow(nSDLow)
{}

auto PerformanceModel::EvaluateTime(float time) -> Status
{
	auto data = sd.Calculate(time);
	if (data.mean == 0)
		return NORMAL;

	if (time <= data.mean - data.sd * nSDHigh)
		return FAST;
	else if (time > data.mean + data.sd * nSDLow)
		return TOOSLOW;

	return NORMAL;
}