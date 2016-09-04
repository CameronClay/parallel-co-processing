#include "StandardDeviation.h"

StandardDeviation::StandardDeviation(uint32_t maxResults)
:
	maxResults(maxResults),
	oldDiffs(std::make_unique<float[]>(maxResults))
{}

auto StandardDeviation::Calculate(float result) -> CalcData
{
	lock.lock();
	times.push_back(result);
	const uint32_t nResults = times.size();

	if (nResults == maxResults)
	{
		double sum = 0;
		resultCount += nResults;

		for (uint32_t i = 0; i < nResults; ++i)
			sum += oldDiffs[i] = times[i] - mean;

		mean += sum / resultCount;

		sum = 0.0;
		for (uint32_t i = 0; i < nResults; ++i)
			//calculate sum of squared variance
			sum += (times[i] - mean) * oldDiffs[i];

		sqDiff += sum;

		sd = sqrt(sqDiff / resultCount);

		times.clear();
	}
	lock.unlock();

	return { mean, sd };
}

uint32_t StandardDeviation::GetMaxResults()
{
	return maxResults;
}