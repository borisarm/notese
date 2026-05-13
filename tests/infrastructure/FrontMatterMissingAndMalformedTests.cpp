#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>

#include "FrontMatter.hpp"

namespace notes::tests {

namespace {

std::filesystem::path tmp_file(const std::string& name) {
    auto p = std::filesystem::temp_directory_path() / name;
    std::filesystem::remove(p);
    return p;
}

} // namespace

TEST(FrontMatterTests, ReadReturnsNulloptForMissingFile) {
    auto missing = std::filesystem::temp_directory_path() / "notese_does_not_exist_xyz.md";
    std::filesystem::remove(missing);
    EXPECT_FALSE(notes::infra::read_front_matter(missing).has_value());
}

TEST(FrontMatterTests, ReadReturnsNulloptWhenFirstLineIsNotDelimiter) {
    auto path = tmp_file("notese_no_front_matter.md");
    std::ofstream(path) << "not a delimiter\nbody\n";
    EXPECT_FALSE(notes::infra::read_front_matter(path).has_value());
    std::filesystem::remove(path);
}

TEST(FrontMatterTests, ReadSkipsLinesWithoutColonSeparator) {
    auto path = tmp_file("notese_malformed_front_matter.md");
    std::ofstream(path) << "---\n"
                        << "title: ok\n"
                        << "no-colon-here\n"
                        << "---\n"
                        << "body content\n";
    auto doc = notes::infra::read_front_matter(path);
    ASSERT_TRUE(doc.has_value());
    EXPECT_EQ(notes::infra::field_or_empty(*doc, "title"), "ok");
    EXPECT_EQ(notes::infra::field_or_empty(*doc, "missing-key"), "");
    EXPECT_NE(doc->content.find("body content"), std::string::npos);
    std::filesystem::remove(path);
}

} // namespace notes::tests
