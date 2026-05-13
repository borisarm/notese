#include <gtest/gtest.h>

#include <chrono>
#include <string>

#include "Iso8601.hpp"

namespace notes::tests {

using TP = std::chrono::system_clock::time_point;

TEST(Iso8601EdgeCases, TooShortStringsReturnDefaultTimePoint) {
    EXPECT_EQ(notes::infra::from_iso8601<TP>(""), TP{});
    EXPECT_EQ(notes::infra::from_iso8601<TP>("2030-01-01"), TP{});
}

TEST(Iso8601EdgeCases, MissingSeparatorsReturnDefaultTimePoint) {
    EXPECT_EQ(notes::infra::from_iso8601<TP>("2030/01/01T00:00:00Z"), TP{});
    EXPECT_EQ(notes::infra::from_iso8601<TP>("2030-01-01 00:00:00Z"), TP{});
}

TEST(Iso8601EdgeCases, NonNumericComponentsReturnDefaultTimePoint) {
    EXPECT_EQ(notes::infra::from_iso8601<TP>("aaaa-bb-ccTdd:ee:ffZ"), TP{});
}

TEST(Iso8601EdgeCases, InvalidYearMonthDayReturnsDefaultTimePoint) {
    EXPECT_EQ(notes::infra::from_iso8601<TP>("2030-13-01T00:00:00Z"), TP{});
    EXPECT_EQ(notes::infra::from_iso8601<TP>("2030-02-30T00:00:00Z"), TP{});
}

TEST(Iso8601EdgeCases, MissingTimezoneSpecifierReturnsDefaultTimePoint) {
    EXPECT_EQ(notes::infra::from_iso8601<TP>("2030-01-01T00:00:00X"), TP{});
}

TEST(Iso8601EdgeCases, TrailingGarbageAfterValidStampReturnsDefaultTimePoint) {
    EXPECT_EQ(notes::infra::from_iso8601<TP>("2030-01-01T00:00:00Zextra"), TP{});
}

TEST(Iso8601EdgeCases, FractionalSecondsParseAndRoundTrip) {
    auto tp = notes::infra::from_iso8601<TP>("2030-01-01T00:00:00.123456789Z");
    EXPECT_NE(tp, TP{});
    auto out = notes::infra::to_iso8601(tp);
    EXPECT_FALSE(out.empty());
}

TEST(Iso8601EdgeCases, FractionalSectionWithoutDigitsReturnsDefaultTimePoint) {
    EXPECT_EQ(notes::infra::from_iso8601<TP>("2030-01-01T00:00:00.Z"), TP{});
}

TEST(Iso8601EdgeCases, PositiveAndNegativeTimezoneOffsetsAreParsed) {
    auto plus = notes::infra::from_iso8601<TP>("2030-01-01T12:00:00+02:00");
    auto minus = notes::infra::from_iso8601<TP>("2030-01-01T12:00:00-02:00");
    EXPECT_NE(plus, TP{});
    EXPECT_NE(minus, TP{});
    EXPECT_NE(plus, minus);
}

TEST(Iso8601EdgeCases, MalformedOffsetIsRejected) {
    EXPECT_EQ(notes::infra::from_iso8601<TP>("2030-01-01T12:00:00+0200"), TP{});
    EXPECT_EQ(notes::infra::from_iso8601<TP>("2030-01-01T12:00:00+aa:bb"), TP{});
}

} // namespace notes::tests
