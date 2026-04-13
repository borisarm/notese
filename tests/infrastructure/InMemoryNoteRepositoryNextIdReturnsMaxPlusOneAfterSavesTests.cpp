#include <gtest/gtest.h>

#include "TestCommon.hpp"

namespace notes::tests {

TEST(InMemoryNoteRepositoryTests, NextIdReturnsMaxPlusOneAfterSaves) {
    IntInMemoryRepo repo;
    repo.save(IntNoteType::CreateNew(IntegerId{3}, "Note 3", "Content"));
    repo.save(IntNoteType::CreateNew(IntegerId{1}, "Note 1", "Content"));

    auto id = repo.next_id();

    EXPECT_EQ(id.value, 4);
}

} // namespace notes::tests
