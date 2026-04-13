#include <gtest/gtest.h>

#include "TestCommon.hpp"

namespace notes::tests {

TEST(MarkdownReminderRepositoryTests, SaveAndGetRoundtripsReminder) {
    TempDir tmp;
    IntReminderMarkdownRepo repo(tmp.path());

    const auto now = IntReminderType::Clock::now();
    const auto remind_at = now + std::chrono::hours(24);
    const IntReminderType reminder{IntegerId{1}, "Buy milk", "From store", remind_at, now, now};

    repo.save(reminder);
    const auto result = repo.get(IntegerId{1});

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->id().to_string(), "1");
    EXPECT_EQ(result->title(), "Buy milk");
    EXPECT_EQ(result->content(), "\nFrom store");
    EXPECT_EQ(result->remind_at(), remind_at);
}

} // namespace notes::tests
