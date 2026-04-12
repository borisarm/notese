#include <gtest/gtest.h>

#include "TestCommon.hpp"

namespace notes::tests {

TEST(NoteTests, WithUpdatedContentPreservesIdentityAndCreatedAt) {
    const auto original = NoteType::CreateNew(StringId{"n-2"}, "Title", "Old content");
    const auto updated = original.with_updated_content("New content");

    EXPECT_EQ(updated.id(), original.id());
    EXPECT_EQ(updated.title(), original.title());
    EXPECT_EQ(updated.content(), "New content");
    EXPECT_EQ(updated.created_at(), original.created_at());
    EXPECT_GE(updated.updated_at(), original.updated_at());
}

} // namespace notes::tests