#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include "BrowseView.hpp"
#include "ConfirmView.hpp"
#include "FormViews.hpp"
#include "TuiState.hpp"

namespace notes::tui {

class ViewRouter {
public:
    ftxui::Element render(TuiState& state,
                          const ftxui::Component& input_title,
                          const ftxui::Component& input_content,
                          const ftxui::Component& input_date,
                          const ftxui::Component& note_menu,
                          const ftxui::Component& reminder_menu) const {
        switch (state.mode) {
            case Mode::AddNote:
            case Mode::EditNote:
                return FormViews::render_note_form(state, input_title, input_content);
            case Mode::AddReminder:
            case Mode::EditReminder:
                return FormViews::render_reminder_form(state, input_title, input_content, input_date);
            case Mode::ConfirmDeleteNote:
                if (state.notes.empty()) {
                    state.mode = Mode::Browse;
                    return ftxui::text("");
                }
                return ConfirmView::render_note_delete(state);
            case Mode::ConfirmDeleteReminder:
                if (state.reminders.empty()) {
                    state.mode = Mode::Browse;
                    return ftxui::text("");
                }
                return ConfirmView::render_reminder_delete(state);
            case Mode::Browse:
            default:
                return BrowseView::render(state, note_menu, reminder_menu);
        }
    }
};

} // namespace notes::tui