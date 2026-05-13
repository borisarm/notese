#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>

#include "IntegerId.hpp"
#include "NextIdAllocator.hpp"

namespace notes::tests {

TEST(NextIdAllocatorEdgeCases, IgnoresNonMarkdownFilesAndNonIntegerStems) {
    auto dir = std::filesystem::temp_directory_path() / "notese_next_id_edge";
    std::filesystem::remove_all(dir);
    std::filesystem::create_directories(dir);

    std::ofstream(dir / "5.md") << "ignored";
    std::ofstream(dir / "ignore.txt") << "ignored";   // wrong extension
    std::ofstream(dir / "abc.md") << "ignored";       // non-integer stem
    std::ofstream(dir / "9notnum.md") << "ignored";   // partially-numeric stem
    std::ofstream(dir / "99999999999999999999.md") << "ignored"; // out_of_range

    auto next = notes::infra::next_integer_id_in_directory<notes::IntegerId>(dir);
    EXPECT_EQ(next.to_string(), "6");

    std::filesystem::remove_all(dir);
}

TEST(NextIdAllocatorEdgeCases, ReturnsOneForEmptyDirectory) {
    auto dir = std::filesystem::temp_directory_path() / "notese_next_id_empty";
    std::filesystem::remove_all(dir);
    std::filesystem::create_directories(dir);

    auto next = notes::infra::next_integer_id_in_directory<notes::IntegerId>(dir);
    EXPECT_EQ(next.to_string(), "1");

    std::filesystem::remove_all(dir);
}

} // namespace notes::tests
