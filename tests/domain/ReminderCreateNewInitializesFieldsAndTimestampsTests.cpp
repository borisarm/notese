#include <gtest/gtest.h>

#include <chrono>

#include "TestCommon.hpp"

namespace notes::tests {

TEST(ReminderTests, CreateNewInitializesFieldsAndTimestamps) {
    const auto before = IntReminderType::Clock::now();
    const auto remind_at = before + std::chrono::hours(24);
    const auto reminder = IntReminderType::CreateNew(IntegerId{1}, "Buy milk", "From the store", remind_at);
    const auto after = IntReminderType::Clock::now();

    EXPECT_EQ(reminder.id().to_string(), "1");
    EXPECT_EQ(reminder.title(), "Buy milk");
    EXPECT_EQ(reminder.content(), "From the store");
    EXPECT_EQ(reminder.remind_at(), remind_at);
    EXPECT_GE(reminder.created_at(), before);
    EXPECT_LE(reminder.created_at(), after);
    EXPECT_EQ(reminder.created_at(), reminder.updated_at());
}

} // namespace notes::tests
