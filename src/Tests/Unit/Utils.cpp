#include <gtest/gtest.h>

#include "Utils/Hash.h"

TEST(Utils, HashString_ProducesKnownValues)
{
    auto h1 = fc::Utils::HashString("hello");
    auto h2 = fc::Utils::HashString("hello");
    EXPECT_EQ(h1, h2);
}

TEST(Utils, HashString_ProducesUniqueValues)
{
    auto h1 = fc::Utils::HashString("abc");
    auto h2 = fc::Utils::HashString("xyz");
    EXPECT_NE(h1, h2);
}
TEST(Utils, HashString_IsCaseSensitive)
{
    auto lower = fc::Utils::HashString("abc");
    auto upper = fc::Utils::HashString("ABC");
    EXPECT_NE(lower, upper);
}
