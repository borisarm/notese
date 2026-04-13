#include <gtest/gtest.h>

#include "TestCommon.hpp"

namespace notes::tests {

TEST(IntegerIdTests, ToStringReturnsUnderlyingValue) {
    IntegerId id{42};

    EXPECT_EQ(id.to_string(), "42");
}

} // namespace notes::tests
