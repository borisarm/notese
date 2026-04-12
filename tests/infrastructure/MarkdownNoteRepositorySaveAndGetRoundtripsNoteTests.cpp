#include <gtest/gtest.h>

#include "TestCommon.hpp"

namespace notes::tests {

TEST(MarkdownNoteRepositoryTests, SaveAndGetRoundtripsNote) {
    TempDir tmp;
    MarkdownRepo repo(tmp.path());

    const auto now = NoteType::Clock::now();
    const NoteType note{StringId{"n-1"}, "Title", "Body content", now, now};

    repo.save(note);
    const auto result = repo.get(StringId{"n-1"});

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->id().to_string(), "n-1");
    EXPECT_EQ(result->title(), "Title");
    EXPECT_EQ(result->content(), "\nBody content");
}

} // namespace notes::tests
