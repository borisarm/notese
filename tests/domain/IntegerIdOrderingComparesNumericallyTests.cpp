#include <gtest/gtest.h>

#include "TestCommon.hpp"

namespace notes::tests {

TEST(IntegerIdTests, OrderingComparesNumerically) {
    const IntegerId a{1};
    const IntegerId b{2};

    EXPECT_TRUE(std::is_lt(a <=> b));
    EXPECT_TRUE(std::is_gt(b <=> a));
    EXPECT_TRUE(std::is_eq(a <=> a));
}

} // namespace notes::tests
