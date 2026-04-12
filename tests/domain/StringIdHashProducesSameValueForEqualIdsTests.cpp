#include <gtest/gtest.h>

#include <functional>

#include "TestCommon.hpp"

namespace notes::tests {

TEST(StringIdTests, HashProducesSameValueForEqualIds) {
    const StringId a{"abc"};
    const StringId b{"abc"};
    const std::hash<StringId> hasher;

    EXPECT_EQ(hasher(a), hasher(b));
}

} // namespace notes::tests
