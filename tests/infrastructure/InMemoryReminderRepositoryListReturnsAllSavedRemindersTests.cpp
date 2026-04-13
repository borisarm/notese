#include <gtest/gtest.h>

#include "TestCommon.hpp"

namespace notes::tests {

TEST(InMemoryReminderRepositoryTests, ListReturnsAllSavedReminders) {
    IntReminderInMemoryRepo repo;
    const auto now = IntReminderType::Clock::now();
    repo.save(IntReminderType::CreateNew(IntegerId{1}, "R1", "Body1", now + std::chrono::hours(24)));
    repo.save(IntReminderType::CreateNew(IntegerId{2}, "R2", "Body2", now + std::chrono::hours(48)));

    const auto result = repo.list();

    EXPECT_EQ(result.size(), 2);
}

} // namespace notes::tests
