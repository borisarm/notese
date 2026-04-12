#include <chrono>
#include <optional>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "Note.hpp"
#include "NoteIdConcept.hpp"
#include "NoteRepositoryConcept.hpp"
#include "StringId.hpp"

namespace notes::tests {

using NoteType = Note<StringId>;

struct InMemoryRepoMock {
    void save(const NoteType&) {}
    void remove(const NoteType::IdType&) {}
    std::optional<NoteType> get(const NoteType::IdType&) const { return std::nullopt; }
    std::vector<NoteType> list() const { return {}; }
};

static_assert(NoteIdConcept<StringId>);
static_assert(NoteRepositoryConcept<InMemoryRepoMock, NoteType>);

TEST(StringIdTests, ToStringReturnsUnderlyingValue) {
    StringId id{"abc-123"};

    EXPECT_EQ(id.to_string(), "abc-123");
}

TEST(NoteTests, CreateNewInitializesFieldsAndTimestamps) {
    const auto before = NoteType::Clock::now();
    const auto note = NoteType::CreateNew(StringId{"n-1"}, "Title", "Body");
    const auto after = NoteType::Clock::now();

    EXPECT_EQ(note.id().to_string(), "n-1");
    EXPECT_EQ(note.title(), "Title");
    EXPECT_EQ(note.content(), "Body");
    EXPECT_GE(note.created_at(), before);
    EXPECT_LE(note.created_at(), after);
    EXPECT_EQ(note.created_at(), note.updated_at());
}

TEST(NoteTests, WithUpdatedContentPreservesIdentityAndCreatedAt) {
    const auto original = NoteType::CreateNew(StringId{"n-2"}, "Title", "Old content");
    const auto updated = original.with_updated_content("New content");

    EXPECT_EQ(updated.id(), original.id());
    EXPECT_EQ(updated.title(), original.title());
    EXPECT_EQ(updated.content(), "New content");
    EXPECT_EQ(updated.created_at(), original.created_at());
    EXPECT_GE(updated.updated_at(), original.updated_at());
}

TEST(NoteTests, WithUpdatedTitlePreservesIdentityAndCreatedAt) {
    const auto original = NoteType::CreateNew(StringId{"n-3"}, "Old title", "Body");
    const auto updated = original.with_updated_title("New title");

    EXPECT_EQ(updated.id(), original.id());
    EXPECT_EQ(updated.title(), "New title");
    EXPECT_EQ(updated.content(), original.content());
    EXPECT_EQ(updated.created_at(), original.created_at());
    EXPECT_GE(updated.updated_at(), original.updated_at());
}

TEST(NoteTests, DefaultComparisonTreatsEqualValuesAsEqual) {
    const auto now = NoteType::Clock::now();
    const NoteType a{StringId{"n-4"}, "Title", "Body", now, now};
    const NoteType b{StringId{"n-4"}, "Title", "Body", now, now};

    EXPECT_TRUE(a == b);
}

} // namespace notes::tests