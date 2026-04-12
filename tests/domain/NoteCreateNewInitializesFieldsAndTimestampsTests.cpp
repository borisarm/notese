#include <gtest/gtest.h>

#include <chrono>

#include "TestCommon.hpp"

namespace notes::tests {

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

} // namespace notes::tests