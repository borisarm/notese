#include <gtest/gtest.h>

#include "TestCommon.hpp"

namespace notes::tests {

TEST(MarkdownNoteRepositoryTests, GetReturnsNulloptForMissing) {
    TempDir tmp;
    const MarkdownRepo repo(tmp.path());

    const auto result = repo.get(StringId{"nonexistent"});

    EXPECT_FALSE(result.has_value());
}

} // namespace notes::tests
