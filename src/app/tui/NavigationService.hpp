#pragma once

#include <algorithm>
#include <format>
#include <ftxui/component/event.hpp>
#include <variant>
#include "TuiState.hpp"

namespace notes::tui {

struct NoAction {};
struct QuitAction {};
struct SaveNoteFormAction {};
struct SaveReminderFormAction {};
struct DeleteSelectedNoteAction {};
struct DeleteSelectedReminderAction {};
struct PassToNoteMenuAction {};
struct PassToReminderMenuAction {};

using NavigationAction = std::variant<
    NoAction,
    QuitAction,
    SaveNoteFormAction,
    SaveReminderFormAction,
    DeleteSelectedNoteAction,
    DeleteSelectedReminderAction,
    PassToNoteMenuAction,
    PassToReminderMenuAction>;

struct NavigationDecision {
    bool handled = false;
    NavigationAction action = NoAction{};
};

class NavigationService {
public:
    NavigationDecision handle_event(const ftxui::Event& event, TuiState& state, bool content_focused) const {
        if (state.mode == Mode::AddNote || state.mode == Mode::EditNote) {
            return handle_note_form_event(event, state, content_focused);
        }
        if (state.mode == Mode::AddReminder || state.mode == Mode::EditReminder) {
            return handle_reminder_form_event(event, state, content_focused);
        }
        if (state.mode == Mode::ConfirmDeleteNote) {
            return handle_note_delete_event(event, state);
        }
        if (state.mode == Mode::ConfirmDeleteReminder) {
            return handle_reminder_delete_event(event, state);
        }
        return handle_browse_event(event, state);
    }

private:
    template <typename Action>
    static NavigationDecision make_handled(Action action) {
        return NavigationDecision{true, NavigationAction{std::move(action)}};
    }

    static NavigationDecision make_unhandled() {
        return NavigationDecision{false, NavigationAction{NoAction{}}};
    }

    static NavigationDecision handle_note_form_event(const ftxui::Event& event, TuiState& state, bool content_focused) {
        if (event == ftxui::Event::Escape) {
            state.mode = Mode::Browse;
            return make_handled(NoAction{});
        }

        if (event != ftxui::Event::Return) {
            return make_unhandled();
        }

        if (content_focused) {
            return make_unhandled();
        }

        if (!state.form_title.empty()) {
            return make_handled(SaveNoteFormAction{});
        }

        return make_handled(NoAction{});
    }

    static NavigationDecision handle_reminder_form_event(const ftxui::Event& event, TuiState& state, bool content_focused) {
        if (event == ftxui::Event::Escape) {
            state.mode = Mode::Browse;
            return make_handled(NoAction{});
        }

        if (event != ftxui::Event::Return) {
            return make_unhandled();
        }

        if (content_focused) {
            return make_unhandled();
        }

        if (!state.form_title.empty() && !state.form_date.empty()) {
            return make_handled(SaveReminderFormAction{});
        }

        return make_handled(NoAction{});
    }

    static NavigationDecision handle_note_delete_event(const ftxui::Event& event, TuiState& state) {
        if (event == ftxui::Event::Character('y')) {
            return make_handled(DeleteSelectedNoteAction{});
        }

        if (event == ftxui::Event::Character('n') || event == ftxui::Event::Escape) {
            state.mode = Mode::Browse;
            return make_handled(NoAction{});
        }

        return make_handled(NoAction{});
    }

    static NavigationDecision handle_reminder_delete_event(const ftxui::Event& event, TuiState& state) {
        if (event == ftxui::Event::Character('y')) {
            return make_handled(DeleteSelectedReminderAction{});
        }

        if (event == ftxui::Event::Character('n') || event == ftxui::Event::Escape) {
            state.mode = Mode::Browse;
            return make_handled(NoAction{});
        }

        return make_handled(NoAction{});
    }

    static NavigationDecision handle_browse_event(const ftxui::Event& event, TuiState& state) {
        if (event == ftxui::Event::Character('q') || event == ftxui::Event::Escape) {
            return make_handled(QuitAction{});
        }

        if (event == ftxui::Event::Tab) {
            state.tab = (state.tab + 1) % 2;
            return make_handled(NoAction{});
        }

        if (event == ftxui::Event::Character('a')) {
            state.form_title.clear();
            state.form_content.clear();
            state.form_date.clear();
            state.mode = (state.tab == 0) ? Mode::AddNote : Mode::AddReminder;
            return make_handled(NoAction{});
        }

        if (event == ftxui::Event::Character('e')) {
            if (state.tab == 0 && !state.notes.empty()) {
                int idx = std::clamp(state.note_selected, 0, (int)state.notes.size() - 1);
                state.form_title = state.notes[idx].title();
                state.form_content = state.notes[idx].content();
                state.mode = Mode::EditNote;
            } else if (state.tab == 1 && !state.reminders.empty()) {
                int idx = std::clamp(state.reminder_selected, 0, (int)state.reminders.size() - 1);
                state.form_title = state.reminders[idx].title();
                state.form_content = state.reminders[idx].content();
                state.form_date = std::format("{:%F}", state.reminders[idx].remind_at());
                state.mode = Mode::EditReminder;
            }
            return make_handled(NoAction{});
        }

        if (event == ftxui::Event::Character('d')) {
            if (state.tab == 0 && !state.notes.empty()) {
                state.mode = Mode::ConfirmDeleteNote;
            } else if (state.tab == 1 && !state.reminders.empty()) {
                state.mode = Mode::ConfirmDeleteReminder;
            }
            return make_handled(NoAction{});
        }

        if (state.tab == 0) {
            return make_handled(PassToNoteMenuAction{});
        }

        return make_handled(PassToReminderMenuAction{});
    }
};

} // namespace notes::tui