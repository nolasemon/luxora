#include <gtest/gtest.h>
#include <iostream>
#include <luxora/series.h>

using namespace Luxora;

TEST(TestSeries, CreationUntyped) {
	SeriesUntyped series("Age", std::vector({18, 30, 92, 15, 20}));

	ASSERT_EQ(series.get_name(), "Age");
}

TEST(TestSeries, CreationTyped) {
	Series<int> series({18, 30, 92, 15, 20});

	ASSERT_EQ(series.count(), 5);
	ASSERT_EQ(series.mean(), 35);
	ASSERT_EQ(series.median(), 20);
}

TEST(TestSeries, TypedConversion) {
	ASSERT_NO_THROW({
		SeriesUntyped untyped("Age", std::vector({18, 30, 92, 15, 20}));

		Series<int> series = untyped.typed<int>();

		// ASSERT_EQ(series.count(), 5);
		// ASSERT_EQ(series.mean(), 35);
		// ASSERT_EQ(series.median(), 20);
	});
}