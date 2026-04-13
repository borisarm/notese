#include <gtest/gtest.h>

#include "TestCommon.hpp"

namespace notes::tests {

TEST(InMemoryNoteRepositoryTests, NextIdReturnsOneForEmptyRepo) {
    IntInMemoryRepo repo;

    auto id = repo.next_id();

    EXPECT_EQ(id.value, 1);
}

} // namespace notes::tests
