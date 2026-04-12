#include <gtest/gtest.h>

#include "TestCommon.hpp"

namespace notes::tests {

TEST(NoteTests, DefaultComparisonTreatsEqualValuesAsEqual) {
    const auto now = NoteType::Clock::now();
    const NoteType a{StringId{"n-4"}, "Title", "Body", now, now};
    const NoteType b{StringId{"n-4"}, "Title", "Body", now, now};

    EXPECT_TRUE(a == b);
}

} // namespace notes::tests