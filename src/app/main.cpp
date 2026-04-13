#include <filesystem>
#include <string>
#include <vector>
#include "MarkdownNoteRepository.hpp"
#include "MarkdownReminderRepository.hpp"
#include "Note.hpp"
#include "Reminder.hpp"
#include "IntegerId.hpp"
#include "Cli.hpp"
#include "Tui.hpp"

int main(int argc, char* argv[]) {
    auto home = std::filesystem::path(std::getenv("HOME"));
    auto notese_dir = home / ".notese";

    using NoteType = notes::Note<notes::IntegerId>;
    using ReminderType = notes::Reminder<notes::IntegerId>;

    notes::MarkdownNoteRepository<NoteType> note_repo(notese_dir / "notes");
    notes::MarkdownReminderRepository<ReminderType> reminder_repo(notese_dir / "reminders");

    std::vector<std::string> args(argv + 1, argv + argc);

    int result = notes::cli::run(note_repo, reminder_repo, args);
    if (result == -1) {
        return notes::tui::run(note_repo, reminder_repo);
    }
    return result;
}
