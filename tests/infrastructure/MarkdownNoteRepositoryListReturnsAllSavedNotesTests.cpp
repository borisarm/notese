#include <gtest/gtest.h>

#include <algorithm>

#include "TestCommon.hpp"

namespace notes::tests {

TEST(MarkdownNoteRepositoryTests, ListReturnsAllSavedNotes) {
    TempDir tmp;
    MarkdownRepo repo(tmp.path());

    const auto now = NoteType::Clock::now();
    repo.save(NoteType{StringId{"n-1"}, "First", "Body1", now, now});
    repo.save(NoteType{StringId{"n-2"}, "Second", "Body2", now, now});

    const auto notes = repo.list();

    ASSERT_EQ(notes.size(), 2u);
    EXPECT_TRUE(std::any_of(notes.begin(), notes.end(),
        [](const NoteType& n) { return n.id().to_string() == "n-1"; }));
    EXPECT_TRUE(std::any_of(notes.begin(), notes.end(),
        [](const NoteType& n) { return n.id().to_string() == "n-2"; }));
}

} // namespace notes::tests
