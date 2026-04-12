#include <gtest/gtest.h>

#include "TestCommon.hpp"

namespace notes::tests {

TEST(InMemoryNoteRepositoryTests, GetReturnsNulloptForMissing) {
    const InMemoryRepo repo;

    const auto result = repo.get(StringId{"nonexistent"});

    EXPECT_FALSE(result.has_value());
}

} // namespace notes::tests
