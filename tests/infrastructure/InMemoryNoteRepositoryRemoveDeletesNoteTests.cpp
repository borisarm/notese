#include <gtest/gtest.h>

#include "TestCommon.hpp"

namespace notes::tests {

TEST(InMemoryNoteRepositoryTests, RemoveDeletesNote) {
    InMemoryRepo repo;
    const auto note = NoteType::CreateNew(StringId{"n-1"}, "Title", "Body");
    repo.save(note);

    repo.remove(StringId{"n-1"});

    EXPECT_FALSE(repo.get(StringId{"n-1"}).has_value());
}

} // namespace notes::tests
