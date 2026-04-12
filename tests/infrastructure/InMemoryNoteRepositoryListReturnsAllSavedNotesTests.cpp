#include <gtest/gtest.h>

#include <algorithm>

#include "TestCommon.hpp"

namespace notes::tests {

TEST(InMemoryNoteRepositoryTests, ListReturnsAllSavedNotes) {
    InMemoryRepo repo;
    repo.save(NoteType::CreateNew(StringId{"n-1"}, "First", "Body1"));
    repo.save(NoteType::CreateNew(StringId{"n-2"}, "Second", "Body2"));

    const auto notes = repo.list();

    ASSERT_EQ(notes.size(), 2u);
    EXPECT_TRUE(std::any_of(notes.begin(), notes.end(),
        [](const NoteType& n) { return n.id().to_string() == "n-1"; }));
    EXPECT_TRUE(std::any_of(notes.begin(), notes.end(),
        [](const NoteType& n) { return n.id().to_string() == "n-2"; }));
}

} // namespace notes::tests
