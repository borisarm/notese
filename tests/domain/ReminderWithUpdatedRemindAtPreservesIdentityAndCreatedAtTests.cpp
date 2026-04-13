#include <gtest/gtest.h>

#include <chrono>

#include "TestCommon.hpp"

namespace notes::tests {

TEST(ReminderTests, WithUpdatedRemindAtPreservesIdentityAndCreatedAt) {
    const auto now = IntReminderType::Clock::now();
    const auto old_remind = now + std::chrono::hours(24);
    const auto new_remind = now + std::chrono::hours(48);
    const auto reminder = IntReminderType::CreateNew(IntegerId{1}, "Title", "Body", old_remind);

    const auto updated = reminder.with_updated_remind_at(new_remind);

    EXPECT_EQ(updated.id(), reminder.id());
    EXPECT_EQ(updated.title(), reminder.title());
    EXPECT_EQ(updated.content(), reminder.content());
    EXPECT_EQ(updated.remind_at(), new_remind);
    EXPECT_EQ(updated.created_at(), reminder.created_at());
    EXPECT_GE(updated.updated_at(), reminder.updated_at());
}

} // namespace notes::tests
