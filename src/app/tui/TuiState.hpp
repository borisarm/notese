#pragma once

#include <string>
#include <vector>
#include "TuiTypes.hpp"

namespace notes::tui {

struct FormFields {
    std::string title;
    std::string content;
    std::string date;

    void clear() {
        title.clear();
        content.clear();
        date.clear();
    }
};

struct BrowseSelection {
    int tab = 0;
    int note_selected = 0;
    int reminder_selected = 0;
};

struct TuiState {
    Mode mode = Mode::Browse;
    BrowseSelection selection;
    FormFields form;

    std::vector<NoteType> notes;
    std::vector<std::string> note_entries;
    std::vector<ReminderType> reminders;
    std::vector<std::string> reminder_entries;

    std::string status_message;

    void enforce_invariants() {
        if (mode == Mode::ConfirmDeleteNote && notes.empty()) {
            mode = Mode::Browse;
        }
        if (mode == Mode::ConfirmDeleteReminder && reminders.empty()) {
            mode = Mode::Browse;
        }
    }
};

} // namespace notes::tui
