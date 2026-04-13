#include <gtest/gtest.h>

#include "TestCommon.hpp"

namespace notes::tests {

TEST(IntegerIdTests, FromStringParsesValue) {
    const IntegerId id{std::string("123")};

    EXPECT_EQ(id.value, 123);
    EXPECT_EQ(id.to_string(), "123");
}

} // namespace notes::tests
