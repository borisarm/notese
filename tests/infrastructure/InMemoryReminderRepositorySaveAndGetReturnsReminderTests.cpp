#include <gtest/gtest.h>

#include "TestCommon.hpp"

namespace notes::tests {

TEST(InMemoryReminderRepositoryTests, SaveAndGetReturnsReminder) {
    IntReminderInMemoryRepo repo;
    const auto remind_at = IntReminderType::Clock::now() + std::chrono::hours(24);
    const auto reminder = IntReminderType::CreateNew(IntegerId{1}, "Buy milk", "From store", remind_at);

    repo.save(reminder);
    const auto result = repo.get(IntegerId{1});

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->id(), reminder.id());
    EXPECT_EQ(result->title(), reminder.title());
    EXPECT_EQ(result->content(), reminder.content());
    EXPECT_EQ(result->remind_at(), reminder.remind_at());
}

} // namespace notes::tests
