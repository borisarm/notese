#include <gtest/gtest.h>

#include "TestCommon.hpp"

namespace notes::tests {

TEST(NoteTests, WithUpdatedTitlePreservesIdentityAndCreatedAt) {
    const auto original = NoteType::CreateNew(StringId{"n-3"}, "Old title", "Body");
    const auto updated = original.with_updated_title("New title");

    EXPECT_EQ(updated.id(), original.id());
    EXPECT_EQ(updated.title(), "New title");
    EXPECT_EQ(updated.content(), original.content());
    EXPECT_EQ(updated.created_at(), original.created_at());
    EXPECT_GE(updated.updated_at(), original.updated_at());
}

} // namespace notes::tests