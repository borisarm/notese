#include <gtest/gtest.h>

#include "TestCommon.hpp"

namespace notes::tests {

TEST(MarkdownNoteRepositoryTests, RemoveDeletesFile) {
    TempDir tmp;
    MarkdownRepo repo(tmp.path());

    const auto now = NoteType::Clock::now();
    const NoteType note{StringId{"n-1"}, "Title", "Body", now, now};
    repo.save(note);

    repo.remove(StringId{"n-1"});

    EXPECT_FALSE(repo.get(StringId{"n-1"}).has_value());
    EXPECT_FALSE(std::filesystem::exists(tmp.path() / "n-1.md"));
}

} // namespace notes::tests
