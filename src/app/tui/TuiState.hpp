#pragma once

#include <string>
#include <vector>
#include "TuiTypes.hpp"

namespace notes::tui {

struct TuiState {
    int note_selected = 0;
    int reminder_selected = 0;
    int tab = 0;
    Mode mode = Mode::Browse;

    std::string form_title;
    std::string form_content;
    std::string form_date;

    std::vector<NoteType> notes;
    std::vector<std::string> note_entries;
    std::vector<ReminderType> reminders;
    std::vector<std::string> reminder_entries;

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