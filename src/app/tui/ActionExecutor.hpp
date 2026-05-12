#pragma once

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
        };

        return std::visit(visitor, decision.action);
    }
};

} // namespace notes::tui