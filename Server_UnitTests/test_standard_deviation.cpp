#include "../Server/StandardDeviation.h"
#include "../Server/StandardDeviation.cpp"
#include "gtest/gtest.h"

TEST(StandardDeviation, Test_OneResult) {
	StandardDeviation std{ 1 };
	const auto res = std.Calculate(100.0f);
	ASSERT_FLOAT_EQ(res.mean, 100.0);
	ASSERT_FLOAT_EQ(res.sd, 0.0);
}

TEST(StandardDeviation, Test_TwoResults) {
	StandardDeviation std{ 2 };
	std.Calculate(100.0f);
	const auto res = std.Calculate(150.0f);
	ASSERT_DOUBLE_EQ(res.mean, 125.0);
	ASSERT_DOUBLE_EQ(res.sd, 25.0);
}