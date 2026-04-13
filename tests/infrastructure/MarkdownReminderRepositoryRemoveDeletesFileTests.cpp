#include <gtest/gtest.h>

#include "TestCommon.hpp"

namespace notes::tests {

TEST(MarkdownReminderRepositoryTests, RemoveDeletesFile) {
    TempDir tmp;
    IntReminderMarkdownRepo repo(tmp.path());

    const auto now = IntReminderType::Clock::now();
    const auto remind_at = now + std::chrono::hours(24);
    repo.save(IntReminderType{IntegerId{1}, "Title", "Body", remind_at, now, now});

    repo.remove(IntegerId{1});

    EXPECT_FALSE(repo.get(IntegerId{1}).has_value());
}

} // namespace notes::tests
