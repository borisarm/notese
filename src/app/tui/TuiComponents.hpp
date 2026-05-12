#pragma once

#include <ftxui/component/component.hpp>
#include "TuiState.hpp"

namespace notes::tui {

struct TuiComponents {
    ftxui::Component input_title;
    ftxui::Component input_content;
    ftxui::Component input_date;
    ftxui::Component note_menu;
    ftxui::Component reminder_menu;
    ftxui::Component form_note_container;
    ftxui::Component form_reminder_container;

    explicit TuiComponents(TuiState& state)
        : input_title(ftxui::Input(&state.form_title, "Title")),
          input_content(ftxui::Input(&state.form_content, "Content")),
          input_date(ftxui::Input(&state.form_date, "YYYY-MM-DD")),
          note_menu(ftxui::Menu(&state.note_entries, &state.note_selected)),
          reminder_menu(ftxui::Menu(&state.reminder_entries, &state.reminder_selected)),
          form_note_container(ftxui::Container::Vertical({input_title, input_content})),
          form_reminder_container(ftxui::Container::Vertical({input_title, input_content, input_date})) {}
};

} // namespace notes::tui
