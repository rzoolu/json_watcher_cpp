#include <gtest/gtest.h>

TEST(FirstTS, passing)
{
    ASSERT_EQ(0, 0);
}

TEST(FirstTS, failing)
{
    ASSERT_EQ(0, 1);
}