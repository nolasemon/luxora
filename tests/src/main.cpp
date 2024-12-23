#include <gtest/gtest.h>
#include <luxora/luxora.h>

#include "series_test.cpp"

using namespace Luxora;

// Additional tests...

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
