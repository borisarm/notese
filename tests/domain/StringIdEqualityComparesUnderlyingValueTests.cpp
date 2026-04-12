#include <gtest/gtest.h>

#include "TestCommon.hpp"

namespace notes::tests {

TEST(StringIdTests, EqualityComparesUnderlyingValue) {
    const StringId a{"abc"};
    const StringId b{"abc"};
    const StringId c{"xyz"};

    EXPECT_EQ(a, b);
    EXPECT_NE(a, c);
}

} // namespace notes::tests
