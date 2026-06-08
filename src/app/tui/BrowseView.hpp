#pragma once

#include <algorithm>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include "Iso8601.hpp"
#include "MarkdownPreview.hpp"
#include "TuiState.hpp"
#include "VisualComponents.hpp"

namespace notes::tui {

class BrowseView {
public:
    static ftxui::Element render(const TuiState& state,
                                 const ftxui::Component& note_menu,
                                 const ftxui::Component& reminder_menu) {
        using namespace ftxui;
        Element detail;

        if (state.selection.tab == 0) {
            detail = render_note_detail(state);
            return vbox({
                visual::app_title(),
                separator(),
                hbox({
                    vbox({
                        text("Notes") | bold | inverted,
                        text("Reminders") | bold | dim,
                        separator(),
                        note_menu->Render() | vscroll_indicator | frame | size(WIDTH, EQUAL, 30),
                    }),
                    separator(),
                    detail | flex,
                }) | flex,
                separator(),
                visual::status_bar(state.status_message),
                visual::help_footer(),
            });
        }

        detail = render_reminder_detail(state);
        return vbox({
            visual::app_title(),
            separator(),
            hbox({
                vbox({
                    text("Notes") | bold | dim,
                    text("Reminders") | bold | inverted,
                    separator(),
                    reminder_menu->Render() | vscroll_indicator | frame | size(WIDTH, EQUAL, 40),
                }),
                separator(),
                detail | flex,
            }) | flex,
            separator(),
            visual::status_bar(state.status_message),
            visual::help_footer(),
        });
    }

private:
    static ftxui::Element render_note_detail(const TuiState& state) {
        using namespace ftxui;
        if (state.notes.empty()) {
            return visual::empty_state("No notes found.", "Press 'a' to add a note.");
        }

        int idx = std::clamp(state.selection.note_selected, 0, (int)state.notes.size() - 1);
        const auto& note = state.notes[idx];
        return vbox({
            text(note.title()) | bold,
            text("id: " + note.id().to_string()) | dim,
            text("updated: " + infra::to_utc_datetime(note.updated_at())) | dim,
            separator(),
            render_markdown_preview(note.content()) | flex,
        }) | flex;
    }

    static ftxui::Element render_reminder_detail(const TuiState& state) {
        using namespace ftxui;
        if (state.reminders.empty()) {
            return visual::empty_state("No reminders found.", "Press 'a' to add a reminder.");
        }

        int idx = std::clamp(state.selection.reminder_selected, 0, (int)state.reminders.size() - 1);
        const auto& reminder = state.reminders[idx];
        return vbox({
            text(reminder.title()) | bold,
            text("id: " + reminder.id().to_string()) | dim,
            text("remind: " + infra::to_utc_date(reminder.remind_at())) | dim,
            text("updated: " + infra::to_utc_datetime(reminder.updated_at())) | dim,
            separator(),
            render_markdown_preview(reminder.content()) | flex,
        }) | flex;
    }
};

} // namespace notes::tui
