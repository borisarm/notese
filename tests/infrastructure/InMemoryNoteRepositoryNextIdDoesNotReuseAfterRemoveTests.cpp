#include <gtest/gtest.h>

#include "TestCommon.hpp"

namespace notes::tests {

TEST(InMemoryNoteRepositoryTests, NextIdDoesNotReuseAfterRemove) {
    IntInMemoryRepo repo;
    auto id1 = repo.next_id();
    repo.save(IntNoteType::CreateNew(id1, "Note 1", "Content"));
    auto id2 = repo.next_id();
    repo.save(IntNoteType::CreateNew(id2, "Note 2", "Content"));

    repo.remove(id2);
    auto id3 = repo.next_id();

    EXPECT_EQ(id3.value, 3);
}

} // namespace notes::tests
