#include <gtest/gtest.h>

#include "TestCommon.hpp"

namespace notes::tests {

TEST(InMemoryReminderRepositoryTests, RemoveDeletesReminder) {
    IntReminderInMemoryRepo repo;
    const auto remind_at = IntReminderType::Clock::now() + std::chrono::hours(24);
    repo.save(IntReminderType::CreateNew(IntegerId{1}, "Title", "Body", remind_at));

    repo.remove(IntegerId{1});

    EXPECT_FALSE(repo.get(IntegerId{1}).has_value());
}

} // namespace notes::tests
