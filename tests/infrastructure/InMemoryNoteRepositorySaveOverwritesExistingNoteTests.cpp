#include <gtest/gtest.h>

#include "TestCommon.hpp"

namespace notes::tests {

TEST(InMemoryNoteRepositoryTests, SaveOverwritesExistingNote) {
    InMemoryRepo repo;
    const auto original = NoteType::CreateNew(StringId{"n-1"}, "Old", "Old body");
    repo.save(original);

    const auto updated = original.with_updated_title("New");
    repo.save(updated);

    const auto result = repo.get(StringId{"n-1"});
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->title(), "New");
}

} // namespace notes::tests
