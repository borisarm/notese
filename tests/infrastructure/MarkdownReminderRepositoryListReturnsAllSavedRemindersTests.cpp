#include <gtest/gtest.h>

#include "TestCommon.hpp"

namespace notes::tests {

TEST(MarkdownReminderRepositoryTests, ListReturnsAllSavedReminders) {
    TempDir tmp;
    IntReminderMarkdownRepo repo(tmp.path());

    const auto now = IntReminderType::Clock::now();
    repo.save(IntReminderType{IntegerId{1}, "R1", "Body1", now + std::chrono::hours(24), now, now});
    repo.save(IntReminderType{IntegerId{2}, "R2", "Body2", now + std::chrono::hours(48), now, now});

    const auto result = repo.list();

    EXPECT_EQ(result.size(), 2);
}

} // namespace notes::tests
