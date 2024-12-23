#include <gtest/gtest.h>
#include <iostream>
#include <luxora/series.h>

using namespace Luxora;

TEST(TestSeries, CreationTyped) {
	Series<int> series({18, 30, 92, 15, 20});

	ASSERT_EQ(series.count(), 5);
	ASSERT_EQ(series.mean(), 35);
	ASSERT_EQ(series.median(), 20);
}

TEST(TestSeries, CreationUntyped) {
	SeriesUntyped series("Age", std::vector({18, 30, 92, 15, 20}));

	ASSERT_EQ(series.get_name(), "Age");

	Series<int>	  typed({18, 30, 92, 15, 20});
	SeriesUntyped series2("Age", typed);
	ASSERT_EQ(series, series2);
}

TEST(TestSeries, TypedConversion) {
	SeriesUntyped untyped("Age", std::vector({18, 30, 92, 15, 20}));

	Series<int> series = untyped.typed<int>();

	ASSERT_EQ(series.count(), 5);
	ASSERT_EQ(series.mean(), 35);
	ASSERT_EQ(series.median(), 20);
}

TEST(TestSeries, WithNA) {
	Series<int> series({18, 30, 92, {}, 20});

	ASSERT_EQ(series.count(), 4);
	ASSERT_EQ(series.mean(), 40);
	ASSERT_EQ(series.median(), 25);
}
