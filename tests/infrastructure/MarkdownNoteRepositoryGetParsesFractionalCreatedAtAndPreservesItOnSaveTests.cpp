#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <string>

#include "TestCommon.hpp"

namespace notes::tests {

TEST(MarkdownNoteRepositoryTests, GetParsesFractionalCreatedAtAndPreservesItOnSave) {
    TempDir tmp;
    const auto file = tmp.path() / "1.md";

    {
        std::ofstream out(file);
        out << "---\n";
        out << "id: 1\n";
        out << "title: Example\n";
        out << "created_at: 2026-05-12T12:13:37.671211Z\n";
        out << "updated_at: 2026-05-12T12:13:37.671211Z\n";
        out << "---\n\n";
        out << "body";
    }

    IntMarkdownRepo repo(tmp.path());
    const auto loaded = repo.get(IntegerId{1});

    ASSERT_TRUE(loaded.has_value());
    EXPECT_GT(loaded->created_at().time_since_epoch().count(), 0);

    const auto updated = loaded->with_updated_content("new body");
    repo.save(updated);

    std::ifstream in(file);
    const std::string saved((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

    EXPECT_NE(saved.find("created_at: 2026-05-12T12:13:37"), std::string::npos);
    EXPECT_EQ(saved.find("created_at: 1970-01-01T00:00:00Z"), std::string::npos);
}

} // namespace notes::tests
