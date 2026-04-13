#include <gtest/gtest.h>

#include "TestCommon.hpp"

namespace notes::tests {

TEST(MarkdownNoteRepositoryTests, SaveAndGetRoundtripsNoteWithIntegerId) {
    TempDir tmp;
    IntMarkdownRepo repo(tmp.path());

    const auto now = IntNoteType::Clock::now();
    const IntNoteType note{IntegerId{42}, "Title", "Body content", now, now};

    repo.save(note);
    const auto result = repo.get(IntegerId{42});

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->id().to_string(), "42");
    EXPECT_EQ(result->title(), "Title");
    EXPECT_EQ(result->content(), "\nBody content");
}

} // namespace notes::tests
