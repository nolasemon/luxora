#include <gtest/gtest.h>
#include <luxora/series.h>

using namespace Luxora;

TEST(TestSeries, CreationTyped) {
	Series<int> series({18, 30, 92, 15, 20});

	ASSERT_EQ(series.size(), 5);
	ASSERT_EQ(series.type(), typeid(int));
	ASSERT_EQ(series.type_size(), 4);

	ASSERT_EQ(series.count(), 5);
	ASSERT_EQ(series.mean(), 35);
	ASSERT_EQ(series.median(), 20);

	Series<int> series2(std::vector<int>{18, 30, 92, 15, 20});

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
	Series<int> series(std::vector<int>{-1, 5, 4, 6, -1, 8});

	series.identify_na(-1);

	ASSERT_EQ(series.size(), 6);
	ASSERT_EQ(series.count(), 4);

	series.fill_na(42);

	ASSERT_EQ(series.size(), 6);
	ASSERT_EQ(series.count(), 6);
}
