#include <gtest/gtest.h>

#include "TestCommon.hpp"

namespace notes::tests {

TEST(MarkdownReminderRepositoryTests, NextIdReturnsOneForEmptyDirectory) {
    TempDir tmp;
    IntReminderMarkdownRepo repo(tmp.path());

    auto id = repo.next_id();

    EXPECT_EQ(id.value, 1);
}

} // namespace notes::tests
