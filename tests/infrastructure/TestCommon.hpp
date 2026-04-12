#pragma once

#include <filesystem>
#include <string>

#include "InMemoryNoteRepository.hpp"
#include "MarkdownNoteRepository.hpp"
#include "Note.hpp"
#include "StringId.hpp"

namespace notes::tests {

using NoteType = Note<StringId>;
using InMemoryRepo = InMemoryNoteRepository<NoteType>;
using MarkdownRepo = MarkdownNoteRepository<NoteType>;

class TempDir {
public:
    TempDir()
        : path_(std::filesystem::temp_directory_path() /
                ("notese_test_" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()))) {
        std::filesystem::create_directories(path_);
    }

    ~TempDir() {
        std::filesystem::remove_all(path_);
    }

    TempDir(const TempDir&) = delete;
    TempDir& operator=(const TempDir&) = delete;

    const std::filesystem::path& path() const { return path_; }

private:
    std::filesystem::path path_;
};

} // namespace notes::tests
