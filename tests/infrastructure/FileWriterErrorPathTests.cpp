#include <gtest/gtest.h>

#include "FileWriter.hpp"

namespace notes::tests {

TEST(FileWriterTests, OpenForWriteThrowsWhenParentDirectoryDoesNotExist) {
    EXPECT_THROW(notes::infra::open_for_write(
                     "/nonexistent_directory_for_notese_tests_1234/output.md"),
                 notes::infra::FileWriteError);
}

TEST(FileWriterTests, OpenForWriteSucceedsForExistingDirectory) {
    auto path = std::filesystem::temp_directory_path() / "notese_filewriter_ok.md";
    std::filesystem::remove(path);
    {
        auto out = notes::infra::open_for_write(path);
        out << "ok";
    }
    EXPECT_TRUE(std::filesystem::exists(path));
    std::filesystem::remove(path);
}

} // namespace notes::tests
