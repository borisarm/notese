#pragma once

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
struct EnterBrowseModeAction {};
struct SwitchTabAction {};
struct BeginAddNoteAction {};
struct BeginAddReminderAction {};
struct BeginEditNoteAction {};
struct BeginEditReminderAction {};
struct EnterConfirmDeleteNoteAction {};
struct EnterConfirmDeleteReminderAction {};

using NavigationAction = std::variant<
    NoAction,
    QuitAction,
    SaveNoteFormAction,
    SaveReminderFormAction,
    DeleteSelectedNoteAction,
    DeleteSelectedReminderAction,
    PassToNoteMenuAction,
    PassToReminderMenuAction,
    EnterBrowseModeAction,
    SwitchTabAction,
    BeginAddNoteAction,
    BeginAddReminderAction,
    BeginEditNoteAction,
    BeginEditReminderAction,
    EnterConfirmDeleteNoteAction,
    EnterConfirmDeleteReminderAction>;

struct NavigationDecision {
    bool handled = false;
    NavigationAction action = NoAction{};
};

class NavigationService {
public:
    NavigationDecision handle_event(const ftxui::Event& event, const TuiState& state, bool content_focused) const {
        if (state.mode == Mode::AddNote || state.mode == Mode::EditNote) {
            return handle_note_form_event(event, state, content_focused);
        }
        if (state.mode == Mode::AddReminder || state.mode == Mode::EditReminder) {
            return handle_reminder_form_event(event, state, content_focused);
        }
        if (state.mode == Mode::ConfirmDeleteNote) {
            return handle_note_delete_event(event);
        }
        if (state.mode == Mode::ConfirmDeleteReminder) {
            return handle_reminder_delete_event(event);
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

    static NavigationDecision handle_note_form_event(const ftxui::Event& event, const TuiState& state, bool content_focused) {
        if (event == ftxui::Event::Escape) {
            return make_handled(EnterBrowseModeAction{});
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

    static NavigationDecision handle_reminder_form_event(const ftxui::Event& event, const TuiState& state, bool content_focused) {
        if (event == ftxui::Event::Escape) {
            return make_handled(EnterBrowseModeAction{});
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

    static NavigationDecision handle_note_delete_event(const ftxui::Event& event) {
        if (event == ftxui::Event::Character('y')) {
            return make_handled(DeleteSelectedNoteAction{});
        }

        if (event == ftxui::Event::Character('n') || event == ftxui::Event::Escape) {
            return make_handled(EnterBrowseModeAction{});
        }

        return make_handled(NoAction{});
    }

    static NavigationDecision handle_reminder_delete_event(const ftxui::Event& event) {
        if (event == ftxui::Event::Character('y')) {
            return make_handled(DeleteSelectedReminderAction{});
        }

        if (event == ftxui::Event::Character('n') || event == ftxui::Event::Escape) {
            return make_handled(EnterBrowseModeAction{});
        }

        return make_handled(NoAction{});
    }

    static NavigationDecision handle_browse_event(const ftxui::Event& event, const TuiState& state) {
        if (event == ftxui::Event::Character('q') || event == ftxui::Event::Escape) {
            return make_handled(QuitAction{});
        }

        if (event == ftxui::Event::Tab) {
            return make_handled(SwitchTabAction{});
        }

        if (event == ftxui::Event::Character('a')) {
            if (state.tab == 0) {
                return make_handled(BeginAddNoteAction{});
            }
            return make_handled(BeginAddReminderAction{});
        }

        if (event == ftxui::Event::Character('e')) {
            if (state.tab == 0 && !state.notes.empty()) {
                return make_handled(BeginEditNoteAction{});
            }
            if (state.tab == 1 && !state.reminders.empty()) {
                return make_handled(BeginEditReminderAction{});
            }
            return make_handled(NoAction{});
        }

        if (event == ftxui::Event::Character('d')) {
            if (state.tab == 0 && !state.notes.empty()) {
                return make_handled(EnterConfirmDeleteNoteAction{});
            }
            if (state.tab == 1 && !state.reminders.empty()) {
                return make_handled(EnterConfirmDeleteReminderAction{});
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
