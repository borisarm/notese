#include <gtest/gtest.h>

#include "TestCommon.hpp"

namespace notes::tests {

TEST(StringIdTests, ToStringReturnsUnderlyingValue) {
    StringId id{"abc-123"};

    EXPECT_EQ(id.to_string(), "abc-123");
}

} // namespace notes::tests