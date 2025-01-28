#include <gtest/gtest.h>
#include <luxora/fixed_string.h>

using namespace Luxora;

TEST(TestFixedString, Creation) {
    FixedString<6> s("hello");
    EXPECT_ANY_THROW(MiniString("12345678901234567890"));
}

TEST(TestFixedString, Operations) {
    String s = "123456";
    EXPECT_EQ(s, String("123456"));
    s = "";
    EXPECT_EQ(s, String(""));
    EXPECT_NE(s, String("123456"));
}

TEST(TestFixedString, Cast) {
    MiniString m       = "123456";
    String     bigger  = m;
    MiniString smaller = bigger;
    String     s       = "12345678901234567890";
    EXPECT_ANY_THROW(MiniString too_small = s);
}
