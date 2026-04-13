#include <gtest/gtest.h>

#include <chrono>

#include "TestCommon.hpp"

namespace notes::tests {

TEST(NoteTests, CreateNewWithIntegerIdInitializesFieldsAndTimestamps) {
    const auto before = IntNoteType::Clock::now();
    const auto note = IntNoteType::CreateNew(IntegerId{1}, "Title", "Body");
    const auto after = IntNoteType::Clock::now();

    EXPECT_EQ(note.id().to_string(), "1");
    EXPECT_EQ(note.title(), "Title");
    EXPECT_EQ(note.content(), "Body");
    EXPECT_GE(note.created_at(), before);
    EXPECT_LE(note.created_at(), after);
    EXPECT_EQ(note.created_at(), note.updated_at());
}

} // namespace notes::tests
