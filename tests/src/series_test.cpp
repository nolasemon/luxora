#include <functional>
#include <gtest/gtest.h>
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
	Series<float> series_float = series_int.easy_convert<float>();
	ASSERT_EQ(series_float.mean(), 40.f);

	Series<std::string> series_string = series_int.convert<std::string>(int2string);
	Series<int>			series2		  = series_string.convert<int>(string2int);
	ASSERT_EQ(series_int, series2);
}
