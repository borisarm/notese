#pragma once

#include <algorithm>
#include <format>
#include <functional>
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <variant>
#include "NavigationService.hpp"
#include "TuiState.hpp"

namespace notes::tui {

template <typename... Func>
struct overload : Func... {
    using Func::operator()...;
};

template <typename... Func>
overload(Func...) -> overload<Func...>;

using EventSink = std::function<bool(const ftxui::Event&)>;

class ActionExecutor {
public:
    template <typename NoteWorkflow, typename ReminderWorkflow>
    bool execute(const NavigationDecision& decision,
                 const ftxui::Event& event,
                 ftxui::ScreenInteractive& screen,
                 TuiState& state,
                 NoteWorkflow& note_workflow,
                 ReminderWorkflow& reminder_workflow,
                 const EventSink& note_event_sink,
                 const EventSink& reminder_event_sink) const {
        if (!decision.handled) {
            return false;
        }

        auto visitor = overload{
            [&](NoAction) -> bool { return true; },
            [&](QuitAction) -> bool {
                screen.Exit();
                return true;
            },
            [&](SaveNoteFormAction) -> bool {
                note_workflow.save_form(state);
                return true;
            },
            [&](SaveReminderFormAction) -> bool {
                reminder_workflow.save_form(state);
                return true;
            },
            [&](DeleteSelectedNoteAction) -> bool {
                note_workflow.delete_selected(state);
                return true;
            },
            [&](DeleteSelectedReminderAction) -> bool {
                reminder_workflow.delete_selected(state);
                return true;
            },
            [&](PassToNoteMenuAction) -> bool {
                return note_event_sink(event);
            },
            [&](PassToReminderMenuAction) -> bool {
                return reminder_event_sink(event);
            },
            [&](EnterBrowseModeAction) -> bool {
                state.mode = Mode::Browse;
                return true;
            },
            [&](SwitchTabAction) -> bool {
                state.tab = (state.tab + 1) % 2;
                return true;
            },
            [&](BeginAddNoteAction) -> bool {
                state.form_title.clear();
                state.form_content.clear();
                state.form_date.clear();
                state.mode = Mode::AddNote;
                return true;
            },
            [&](BeginAddReminderAction) -> bool {
                state.form_title.clear();
                state.form_content.clear();
                state.form_date.clear();
                state.mode = Mode::AddReminder;
                return true;
            },
            [&](BeginEditNoteAction) -> bool {
                if (state.notes.empty()) {
                    return true;
                }
                int idx = std::clamp(state.note_selected, 0, (int)state.notes.size() - 1);
                state.form_title = state.notes[idx].title();
                state.form_content = state.notes[idx].content();
                state.mode = Mode::EditNote;
                return true;
            },
            [&](BeginEditReminderAction) -> bool {
                if (state.reminders.empty()) {
                    return true;
                }
                int idx = std::clamp(state.reminder_selected, 0, (int)state.reminders.size() - 1);
                state.form_title = state.reminders[idx].title();
                state.form_content = state.reminders[idx].content();
                state.form_date = std::format("{:%F}", state.reminders[idx].remind_at());
                state.mode = Mode::EditReminder;
                return true;
            },
            [&](EnterConfirmDeleteNoteAction) -> bool {
                state.mode = Mode::ConfirmDeleteNote;
                return true;
            },
            [&](EnterConfirmDeleteReminderAction) -> bool {
                state.mode = Mode::ConfirmDeleteReminder;
                return true;
            },
        };

        const bool handled = std::visit(visitor, decision.action);
        state.enforce_invariants();
        return handled;
    }
};

} // namespace notes::tui
