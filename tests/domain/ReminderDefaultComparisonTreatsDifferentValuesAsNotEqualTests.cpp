#include <gtest/gtest.h>

#include <chrono>

#include "TestCommon.hpp"

namespace notes::tests {

TEST(ReminderTests, DefaultComparisonTreatsDifferentValuesAsNotEqual) {
    const auto now = IntReminderType::Clock::now();
    const auto remind_at = now + std::chrono::hours(24);
    const IntReminderType a{IntegerId{1}, "Title", "Body", remind_at, now, now};
    const IntReminderType b{IntegerId{2}, "Title", "Body", remind_at, now, now};
    const IntReminderType c{IntegerId{1}, "Other", "Body", remind_at, now, now};

    EXPECT_NE(a, b);
    EXPECT_NE(a, c);
}

} // namespace notes::tests
