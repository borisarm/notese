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
};

} // namespace notes::tui