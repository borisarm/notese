#include <gtest/gtest.h>

#include <chrono>

#include "TestCommon.hpp"

namespace notes::tests {

TEST(ReminderTests, WithUpdatedContentPreservesIdentityAndCreatedAt) {
    const auto remind_at = IntReminderType::Clock::now() + std::chrono::hours(24);
    const auto reminder = IntReminderType::CreateNew(IntegerId{1}, "Title", "Old", remind_at);

    const auto updated = reminder.with_updated_content("New");

    EXPECT_EQ(updated.id(), reminder.id());
    EXPECT_EQ(updated.title(), reminder.title());
    EXPECT_EQ(updated.content(), "New");
    EXPECT_EQ(updated.remind_at(), remind_at);
    EXPECT_EQ(updated.created_at(), reminder.created_at());
    EXPECT_GE(updated.updated_at(), reminder.updated_at());
}

} // namespace notes::tests
