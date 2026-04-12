#include <gtest/gtest.h>

#include "TestCommon.hpp"

namespace notes::tests {

TEST(NoteTests, DefaultComparisonTreatsDifferentValuesAsNotEqual) {
    const auto now = NoteType::Clock::now();
    const NoteType a{StringId{"n-1"}, "Title A", "Body", now, now};
    const NoteType b{StringId{"n-2"}, "Title B", "Body", now, now};

    EXPECT_FALSE(a == b);
}

} // namespace notes::tests
