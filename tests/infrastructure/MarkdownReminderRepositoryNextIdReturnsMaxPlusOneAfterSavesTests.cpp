#include <gtest/gtest.h>

#include "TestCommon.hpp"

namespace notes::tests {

TEST(MarkdownReminderRepositoryTests, NextIdReturnsMaxPlusOneAfterSaves) {
    TempDir tmp;
    IntReminderMarkdownRepo repo(tmp.path());

    const auto now = IntReminderType::Clock::now();
    repo.save(IntReminderType{IntegerId{3}, "R3", "Body", now + std::chrono::hours(24), now, now});
    repo.save(IntReminderType{IntegerId{1}, "R1", "Body", now + std::chrono::hours(48), now, now});

    auto id = repo.next_id();

    EXPECT_EQ(id.value, 4);
}

} // namespace notes::tests
