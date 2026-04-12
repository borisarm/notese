#include <gtest/gtest.h>

#include "TestCommon.hpp"

namespace notes::tests {

TEST(StringIdTests, OrderingComparesLexicographically) {
    const StringId a{"aaa"};
    const StringId b{"bbb"};

    EXPECT_TRUE(std::is_lt(a <=> b));
    EXPECT_TRUE(std::is_gt(b <=> a));
    EXPECT_TRUE(std::is_eq(a <=> a));
}

} // namespace notes::tests
