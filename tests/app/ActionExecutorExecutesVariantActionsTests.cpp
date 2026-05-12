#include <gtest/gtest.h>

#include "../../src/app/tui/ActionExecutor.hpp"

namespace notes::tests {

namespace {

struct NoteWorkflowStub {
    bool save_called = false;
    bool delete_called = false;

    void save_form(notes::tui::TuiState&) { save_called = true; }
    void delete_selected(notes::tui::TuiState&) { delete_called = true; }
};

struct ReminderWorkflowStub {
    bool save_called = false;
    bool delete_called = false;

    void save_form(notes::tui::TuiState&) { save_called = true; }
    void delete_selected(notes::tui::TuiState&) { delete_called = true; }
};

} // namespace

TEST(ActionExecutorTests, SaveNoteFormActionCallsNoteWorkflow) {
    notes::tui::ActionExecutor executor;
    notes::tui::TuiState state;
    NoteWorkflowStub note_workflow;
    ReminderWorkflowStub reminder_workflow;

    std::vector<std::string> entries;
    int selected = 0;
    auto note_menu = ftxui::Menu(&entries, &selected);
    auto reminder_menu = ftxui::Menu(&entries, &selected);

    auto screen = ftxui::ScreenInteractive::FitComponent();

    notes::tui::NavigationDecision decision{true, notes::tui::SaveNoteFormAction{}};
    const auto handled = executor.execute(
        decision,
        ftxui::Event::Custom,
        screen,
        state,
        note_workflow,
        reminder_workflow,
        note_menu,
        reminder_menu);

    EXPECT_TRUE(handled);
    EXPECT_TRUE(note_workflow.save_called);
    EXPECT_FALSE(reminder_workflow.save_called);
}

TEST(ActionExecutorTests, DeleteReminderActionCallsReminderWorkflow) {
    notes::tui::ActionExecutor executor;
    notes::tui::TuiState state;
    NoteWorkflowStub note_workflow;
    ReminderWorkflowStub reminder_workflow;

    std::vector<std::string> entries;
    int selected = 0;
    auto note_menu = ftxui::Menu(&entries, &selected);
    auto reminder_menu = ftxui::Menu(&entries, &selected);

    auto screen = ftxui::ScreenInteractive::FitComponent();

    notes::tui::NavigationDecision decision{true, notes::tui::DeleteSelectedReminderAction{}};
    const auto handled = executor.execute(
        decision,
        ftxui::Event::Custom,
        screen,
        state,
        note_workflow,
        reminder_workflow,
        note_menu,
        reminder_menu);

    EXPECT_TRUE(handled);
    EXPECT_TRUE(reminder_workflow.delete_called);
    EXPECT_FALSE(note_workflow.delete_called);
}

TEST(ActionExecutorTests, UnhandledDecisionReturnsFalse) {
    notes::tui::ActionExecutor executor;
    notes::tui::TuiState state;
    NoteWorkflowStub note_workflow;
    ReminderWorkflowStub reminder_workflow;

    std::vector<std::string> entries;
    int selected = 0;
    auto note_menu = ftxui::Menu(&entries, &selected);
    auto reminder_menu = ftxui::Menu(&entries, &selected);

    auto screen = ftxui::ScreenInteractive::FitComponent();

    notes::tui::NavigationDecision decision{false, notes::tui::NoAction{}};
    const auto handled = executor.execute(
        decision,
        ftxui::Event::Custom,
        screen,
        state,
        note_workflow,
        reminder_workflow,
        note_menu,
        reminder_menu);

    EXPECT_FALSE(handled);
}

} // namespace notes::tests
