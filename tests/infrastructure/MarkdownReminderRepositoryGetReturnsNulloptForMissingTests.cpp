#include <gtest/gtest.h>

#include "TestCommon.hpp"

namespace notes::tests {

TEST(MarkdownReminderRepositoryTests, GetReturnsNulloptForMissing) {
    TempDir tmp;
    IntReminderMarkdownRepo repo(tmp.path());

    const auto result = repo.get(IntegerId{99});

    EXPECT_FALSE(result.has_value());
}

} // namespace notes::tests
