#pragma once

#include <algorithm>
#include <ftxui/dom/elements.hpp>
#include "TuiState.hpp"

namespace notes::tui {

class ConfirmView {
public:
    static ftxui::Element render_note_delete(const TuiState& state) {
        using namespace ftxui;
        if (state.notes.empty()) {
            return text("");
        }
        auto idx = std::clamp(state.note_selected, 0, (int)state.notes.size() - 1);
        const auto& note = state.notes[idx];
        return vbox({
            text("Delete Note") | bold | center,
            separator(),
            text("Are you sure you want to delete:") | center,
            text("  " + note.title()) | bold | center,
            filler(),
            separator(),
            text("y yes  n/Escape cancel") | dim | center,
        }) | border;
    }

    static ftxui::Element render_reminder_delete(const TuiState& state) {
        using namespace ftxui;
        if (state.reminders.empty()) {
            return text("");
        }
        auto idx = std::clamp(state.reminder_selected, 0, (int)state.reminders.size() - 1);
        const auto& reminder = state.reminders[idx];
        return vbox({
            text("Delete Reminder") | bold | center,
            separator(),
            text("Are you sure you want to delete:") | center,
            text("  " + reminder.title()) | bold | center,
            filler(),
            separator(),
            text("y yes  n/Escape cancel") | dim | center,
        }) | border;
    }
};

} // namespace notes::tui