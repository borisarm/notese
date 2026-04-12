#include <gtest/gtest.h>

#include "TestCommon.hpp"

namespace notes::tests {

TEST(InMemoryNoteRepositoryTests, SaveAndGetReturnsNote) {
    InMemoryRepo repo;
    const auto note = NoteType::CreateNew(StringId{"n-1"}, "Title", "Body");

    repo.save(note);
    const auto result = repo.get(StringId{"n-1"});

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->id(), note.id());
    EXPECT_EQ(result->title(), note.title());
    EXPECT_EQ(result->content(), note.content());
}

} // namespace notes::tests
