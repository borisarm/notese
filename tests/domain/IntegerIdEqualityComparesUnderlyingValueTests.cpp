#include <gtest/gtest.h>

#include "TestCommon.hpp"

namespace notes::tests {

TEST(IntegerIdTests, EqualityComparesUnderlyingValue) {
    const IntegerId a{1};
    const IntegerId b{1};
    const IntegerId c{2};

    EXPECT_EQ(a, b);
    EXPECT_NE(a, c);
}

} // namespace notes::tests
