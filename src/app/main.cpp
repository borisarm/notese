#include <filesystem>
#include <string>
#include <vector>
#include "MarkdownNoteRepository.hpp"
#include "Note.hpp"
#include "IntegerId.hpp"
#include "Cli.hpp"
#include "Tui.hpp"

int main(int argc, char* argv[]) {
    auto home = std::filesystem::path(std::getenv("HOME"));
    auto notes_dir = home / ".notese";

    using NoteType = notes::Note<notes::IntegerId>;
    notes::MarkdownNoteRepository<NoteType> repo(notes_dir);

    std::vector<std::string> args(argv + 1, argv + argc);

    int result = notes::cli::run(repo, args);
    if (result == -1) {
        return notes::tui::run(repo);
    }
    return result;
}
