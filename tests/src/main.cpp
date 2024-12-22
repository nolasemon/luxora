#include <gtest/gtest.h>
#include <luxora/luxora.h>

TEST(CLI11Test, BasicParsing) {
	EXPECT_EQ(Luxora::X, 42);
}

// Additional tests...

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
