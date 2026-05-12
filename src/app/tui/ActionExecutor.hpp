#pragma once

#include <algorithm>
#include <format>
#include <ftxui/component/component.hpp>
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

class ActionExecutor {
public:
    template <typename NoteWorkflow, typename ReminderWorkflow>
    bool execute(const NavigationDecision& decision,
                 const ftxui::Event& event,
                 ftxui::ScreenInteractive& screen,
                 TuiState& state,
                 NoteWorkflow& note_workflow,
                 ReminderWorkflow& reminder_workflow,
                 const ftxui::Component& note_menu,
                 const ftxui::Component& reminder_menu) const {
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
                return note_menu->OnEvent(event);
            },
            [&](PassToReminderMenuAction) -> bool {
                return reminder_menu->OnEvent(event);
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

        return std::visit(visitor, decision.action);
    }
};

} // namespace notes::tui
