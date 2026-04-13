#include <gtest/gtest.h>

#include <functional>

#include "TestCommon.hpp"

namespace notes::tests {

TEST(IntegerIdTests, HashProducesSameValueForEqualIds) {
    const IntegerId a{7};
    const IntegerId b{7};
    const std::hash<IntegerId> hasher;

    EXPECT_EQ(hasher(a), hasher(b));
}

} // namespace notes::tests
