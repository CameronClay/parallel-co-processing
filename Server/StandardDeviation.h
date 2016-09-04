#pragma once
#include <cstdint>
#include <cmath>
#include <memory>
#include <vector>
#include <mutex>

class StandardDeviation
{
public:
	struct CalcData
	{
		double mean, sd;
	};

	StandardDeviation(uint32_t maxResults = 1000);

	CalcData Calculate(float result);
	uint32_t GetMaxResults();
private:
	double mean = 0, sqDiff = 0, sd = 0.0;
	uint64_t resultCount = 0;
	const uint32_t maxResults;
	std::unique_ptr<float[]> oldDiffs;
	std::vector<float> times;
	std::mutex lock;
};