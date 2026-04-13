#include <gtest/gtest.h>

#include <chrono>

#include "TestCommon.hpp"

namespace notes::tests {

TEST(ReminderTests, DefaultComparisonTreatsEqualValuesAsEqual) {
    const auto now = IntReminderType::Clock::now();
    const auto remind_at = now + std::chrono::hours(24);
    const IntReminderType a{IntegerId{1}, "Title", "Body", remind_at, now, now};
    const IntReminderType b{IntegerId{1}, "Title", "Body", remind_at, now, now};

    EXPECT_EQ(a, b);
}

} // namespace notes::tests
