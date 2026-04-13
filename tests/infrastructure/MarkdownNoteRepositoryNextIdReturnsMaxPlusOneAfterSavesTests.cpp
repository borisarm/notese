#include <gtest/gtest.h>

#include "TestCommon.hpp"

namespace notes::tests {

TEST(MarkdownNoteRepositoryTests, NextIdReturnsMaxPlusOneAfterSaves) {
    TempDir tmp;
    IntMarkdownRepo repo(tmp.path());

    const auto now = IntNoteType::Clock::now();
    repo.save(IntNoteType{IntegerId{3}, "Note 3", "Content", now, now});
    repo.save(IntNoteType{IntegerId{1}, "Note 1", "Content", now, now});

    auto id = repo.next_id();

    EXPECT_EQ(id.value, 4);
}

} // namespace notes::tests
