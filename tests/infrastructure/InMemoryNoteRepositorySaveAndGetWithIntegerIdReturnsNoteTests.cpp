#include <gtest/gtest.h>

#include "TestCommon.hpp"

namespace notes::tests {

TEST(InMemoryNoteRepositoryTests, SaveAndGetWithIntegerIdReturnsNote) {
    IntInMemoryRepo repo;
    const auto note = IntNoteType::CreateNew(IntegerId{1}, "Title", "Body");

    repo.save(note);
    const auto result = repo.get(IntegerId{1});

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->id(), note.id());
    EXPECT_EQ(result->title(), note.title());
    EXPECT_EQ(result->content(), note.content());
}

} // namespace notes::tests
