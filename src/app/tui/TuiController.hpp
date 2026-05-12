#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include "ActionExecutor.hpp"
#include "AppRuntime.hpp"
#include "NavigationService.hpp"
#include "NoteRepositoryConcept.hpp"
#include "NoteWorkflowService.hpp"
#include "ReminderWorkflowService.hpp"
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
          input_title_(ftxui::Input(&state_.form_title, "Title")),
          input_content_(ftxui::Input(&state_.form_content, "Content")),
          input_date_(ftxui::Input(&state_.form_date, "YYYY-MM-DD")),
          note_menu_(ftxui::Menu(&state_.note_entries, &state_.note_selected)),
          reminder_menu_(ftxui::Menu(&state_.reminder_entries, &state_.reminder_selected)),
          form_note_container_(ftxui::Container::Vertical({input_title_, input_content_})),
          form_reminder_container_(ftxui::Container::Vertical({input_title_, input_content_, input_date_})),
          screen_(ftxui::ScreenInteractive::Fullscreen()) {}

    int run() {
        note_workflow_.refresh(state_);
        reminder_workflow_.refresh(state_);

        AppRuntime::run(
            screen_,
            {
                form_note_container_,
                form_reminder_container_,
                note_menu_,
                reminder_menu_,
            },
            [&]() -> ftxui::Element {
                return view_router_.render(
                    state_,
                    input_title_,
                    input_content_,
                    input_date_,
                    note_menu_,
                    reminder_menu_);
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

    ftxui::Component input_title_;
    ftxui::Component input_content_;
    ftxui::Component input_date_;
    ftxui::Component note_menu_;
    ftxui::Component reminder_menu_;
    ftxui::Component form_note_container_;
    ftxui::Component form_reminder_container_;
    ftxui::ScreenInteractive screen_;

    bool on_event(ftxui::Event event) {
        auto decision = navigation_.handle_event(event, state_, input_content_->Focused());
        return action_executor_.execute(
            decision,
            event,
            screen_,
            state_,
            note_workflow_,
            reminder_workflow_,
            note_menu_,
            reminder_menu_);
    }
};

} // namespace notes::tui
