#pragma once

#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include "ActionExecutor.hpp"
#include "AppRuntime.hpp"
#include "NavigationService.hpp"
#include "NoteRepositoryConcept.hpp"
#include "NoteWorkflowService.hpp"
#include "ReminderWorkflowService.hpp"
#include "TuiComponents.hpp"
#include "TuiState.hpp"
#include "TuiTypes.hpp"
#include "ViewRouter.hpp"

namespace notes::tui {

template <NoteRepositoryConcept<NoteType> NoteRepo, NoteRepositoryConcept<ReminderType> ReminderRepo>
class TuiController {
public:
    TuiController(NoteRepo& note_repo, ReminderRepo& reminder_repo)
        : note_workflow_(note_repo),
          reminder_workflow_(reminder_repo),
          components_(state_),
          screen_(ftxui::ScreenInteractive::Fullscreen()) {}

    int run() {
        note_workflow_.refresh(state_);
        reminder_workflow_.refresh(state_);

        AppRuntime::run(
            screen_,
            {
                components_.form_note_container,
                components_.form_reminder_container,
                components_.note_menu,
                components_.reminder_menu,
            },
            [&]() -> ftxui::Element {
                state_.enforce_invariants();
                return view_router_.render(
                    state_,
                    components_.input_title,
                    components_.input_content,
                    components_.input_date,
                    components_.note_menu,
                    components_.reminder_menu);
            },
            [&](ftxui::Event event) -> bool { return on_event(event); });

        return 0;
    }

private:
    TuiState state_;
    ViewRouter view_router_;
    NavigationService navigation_;
    ActionExecutor action_executor_;
    NoteWorkflowService<NoteRepo> note_workflow_;
    ReminderWorkflowService<ReminderRepo> reminder_workflow_;
    TuiComponents components_;
    ftxui::ScreenInteractive screen_;

    bool on_event(ftxui::Event event) {
        auto decision = navigation_.handle_event(event, state_, components_.input_content->Focused());
        return action_executor_.execute(
            decision,
            event,
            screen_,
            state_,
            note_workflow_,
            reminder_workflow_,
            components_.note_menu,
            components_.reminder_menu);
    }
};

} // namespace notes::tui
