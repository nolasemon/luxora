#include <functional>
#include <gtest/gtest.h>
#include <iostream>
#include <luxora/series.h>
#include <string>

using namespace Luxora;

TEST(TestSeries, CreationTyped) {
	Series<int> series({18, 30, 92, 15, 20});

	ASSERT_EQ(series.size(), 5);
	ASSERT_EQ(series.type(), typeid(int));
	ASSERT_EQ(series.type_size(), 4);

	ASSERT_EQ(series.count(), 5);
	ASSERT_EQ(series.mean(), 35);
	ASSERT_EQ(series.median(), 20);

	Series<int> series2 = Series<int>::from_vector({18, 30, 92, 15, 20});

	ASSERT_EQ(series.size(), 5);
	ASSERT_EQ(series.type(), typeid(int));
	ASSERT_EQ(series.type_size(), 4);

	ASSERT_EQ(series.count(), 5);
	ASSERT_EQ(series.mean(), 35);
	ASSERT_EQ(series.median(), 20);
}

TEST(TestSeries, WithNA) {
	Series<int> series({18, 30, 92, {}, 20});

	ASSERT_EQ(series.size(), 5);
	ASSERT_EQ(series.type(), typeid(int));
	ASSERT_EQ(series.type_size(), 4);

	ASSERT_EQ(series.count(), 4);
	ASSERT_EQ(series.mean(), 40);
	ASSERT_EQ(series.median(), 25);
}

TEST(TestSeries, TestFillNa) {
	Series<int> series({18, 30, 92, {}, 20});

	series.fill_na(40);
	ASSERT_EQ(series.count(), 5);
	ASSERT_EQ(series.mean(), 40);
	ASSERT_EQ(series.median(), 30);

	series.fill_na(100000000);
	ASSERT_EQ(series.count(), 5);
	ASSERT_EQ(series.mean(), 40);
	ASSERT_EQ(series.median(), 30);

	Series<int> series2({18, 30, 92, {}, 20});

	series2.fill_na(25);
	ASSERT_EQ(series2.count(), 5);
	ASSERT_EQ(series2.mean(), 37);
	ASSERT_EQ(series2.median(), 25);
}

TEST(TestSeries, TestIdentifyNa) {
	Series<int> series = Series<int>::from_vector({-1, 5, 4, 6, -1, 8});

	series.identify_na(-1);

	ASSERT_EQ(series.size(), 6);
	ASSERT_EQ(series.count(), 4);

	series.fill_na(42);

	ASSERT_EQ(series.size(), 6);
	ASSERT_EQ(series.count(), 6);
}

TEST(TestSeries, TestConvert) {
	Series<int>	  series_int({18, 30, 92, {}, 20});
	Series<float> series_float = series_int.cast<float>();
	ASSERT_EQ(series_float.mean(), 40.f);

	Series<std::string> series_string = series_int.map<std::string>(int2string);
	Series<int>			series2		  = series_string.map<int>(string2int);
	ASSERT_EQ(series_int, series2);
}

TEST(TestSeries, TestMap) {
	Series<int> series_int({18, 30, 92, {}, 20});

	Series<float> series_float = series_int.map<float>([](const int& x) { return float(x) / 3; });
	ASSERT_EQ(series_float, Series<float>({6.f, 10.f, 30.f + 2.f / 3, {}, 6 + 2.f / 3}));

	Series<std::string> concat = series_int.map<std::string>([](const int& x) { return std::to_string(x) + "be"; });
	ASSERT_EQ(concat, Series<std::string>({"18be", "30be", "92be", {}, "20be"}));

	concat.map_inplace([](const std::string& s) { return s + std::string("ans"); });
	ASSERT_EQ(concat, Series<std::string>({"18beans", "30beans", "92beans", {}, "20beans"}));

	// TODO safe invocation
	// Series<float> erroneous = series_int.map<int>([](const int& x) { return 10 / (x - 30); });
}

TEST(TestSeries, StatisticalFunctions) {
	Series<float> s = Series<float>::from_vector({1, 2, 3, 4, 5});

	ASSERT_EQ(s.sum(), 15);
	ASSERT_EQ(s.mean(), 3);
	ASSERT_EQ(s.median(), 3);

	ASSERT_EQ(s.max(), 5);
	ASSERT_EQ(s.min(), 1);
	ASSERT_EQ(s.range(), 4);

	ASSERT_EQ(s.quantile(0.25), 2);
	ASSERT_EQ(s.quantile(0.75), 4);
	ASSERT_EQ(s.iqr(), 2);

	ASSERT_EQ(s.variance(), 2);
	ASSERT_NEAR(s.stddev(), std::sqrt(2), 1e-5);

	ASSERT_EQ(s.count(), 5);
}

TEST(TestSeries, Normalization) {
	Series<float> s					= Series<float>::from_vector({1, 2, 3, 4, 5});
	auto		  normalized_minmax = s.normalized_minmax();
	ASSERT_EQ(normalized_minmax.min(), 0.0);
	ASSERT_EQ(normalized_minmax.max(), 1.0);

	auto normalized_zscore = s.normalized_zscore();
	ASSERT_NEAR(normalized_zscore.mean(), 0.0, 1e-5);
	ASSERT_NEAR(normalized_zscore.stddev(), 1.0, 1e-5);
}

TEST(TestSeries, Outliers) {
	Series<int> s				= Series<int>::from_vector({1, 2, 3, 100, 5});
	auto		outliers		= s.outliers();
	auto		outlier_indices = s.outlier_indices();

	ASSERT_EQ(outliers.size(), 1);
	ASSERT_EQ(outliers[0], 100);

	ASSERT_EQ(outlier_indices.size(), 1);
	ASSERT_EQ(outlier_indices[0], 3);
}
