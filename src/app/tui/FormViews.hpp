#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include "MarkdownPreview.hpp"
#include "TuiState.hpp"

namespace notes::tui {

class FormViews {
public:
    static ftxui::Element render_note_form(const TuiState& state,
                                           const ftxui::Component& input_title,
                                           const ftxui::Component& input_content) {
        using namespace ftxui;
        auto title = (state.mode == Mode::AddNote) ? "Add Note" : "Edit Note";
        auto editor = vbox({
            text(title) | bold | center,
            separator(),
            hbox(text("Title:   "), input_title->Render() | flex),
            separator(),
            hbox(text("Content: "), input_content->Render() | flex),
            filler(),
            separator(),
            text("Enter confirm (except in content)  Escape cancel") | dim | center,
        }) | border | flex;

        auto preview = vbox({
            text("Preview") | bold | center,
            separator(),
            text(state.form.title.empty() ? "(untitled)" : state.form.title) | bold,
            separator(),
            render_markdown_preview(state.form.content) | flex,
        }) | border | flex;

        return hbox({editor, separator(), preview});
    }

    static ftxui::Element render_reminder_form(const TuiState& state,
                                               const ftxui::Component& input_title,
                                               const ftxui::Component& input_content,
                                               const ftxui::Component& input_date) {
        using namespace ftxui;
        auto title = (state.mode == Mode::AddReminder) ? "Add Reminder" : "Edit Reminder";
        auto editor = vbox({
            text(title) | bold | center,
            separator(),
            hbox(text("Title:   "), input_title->Render() | flex),
            hbox(text("Date:    "), input_date->Render() | flex),
            separator(),
            hbox(text("Content: "), input_content->Render() | flex),
            filler(),
            separator(),
            text("Enter confirm (except in content)  Escape cancel") | dim | center,
        }) | border | flex;

        auto preview = vbox({
            text("Preview") | bold | center,
            separator(),
            text(state.form.title.empty() ? "(untitled)" : state.form.title) | bold,
            text(state.form.date.empty() ? "(no date)" : ("date: " + state.form.date)) | dim,
            separator(),
            render_markdown_preview(state.form.content) | flex,
        }) | border | flex;

        return hbox({editor, separator(), preview});
    }
};

} // namespace notes::tui
