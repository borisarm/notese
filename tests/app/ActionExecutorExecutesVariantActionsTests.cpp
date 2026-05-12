#include <gtest/gtest.h>

#include "ActionExecutor.hpp"
#include "IntegerId.hpp"
#include "Note.hpp"

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

struct ExecutorFixture {
    notes::tui::ActionExecutor executor;
    notes::tui::TuiState state;
    NoteWorkflowStub note_workflow;
    ReminderWorkflowStub reminder_workflow;
    ftxui::ScreenInteractive screen = ftxui::ScreenInteractive::FitComponent();
    int note_sink_calls = 0;
    int reminder_sink_calls = 0;

    notes::tui::EventSink note_sink = [this](const ftxui::Event&) {
        ++note_sink_calls;
        return true;
    };
    notes::tui::EventSink reminder_sink = [this](const ftxui::Event&) {
        ++reminder_sink_calls;
        return true;
    };

    bool execute(notes::tui::NavigationAction action) {
        notes::tui::NavigationDecision decision{true, std::move(action)};
        return executor.execute(
            decision,
            ftxui::Event::Custom,
            screen,
            state,
            note_workflow,
            reminder_workflow,
            note_sink,
            reminder_sink);
    }
};

} // namespace

TEST(ActionExecutorTests, SaveNoteFormActionCallsNoteWorkflow) {
    ExecutorFixture fx;

    EXPECT_TRUE(fx.execute(notes::tui::SaveNoteFormAction{}));
    EXPECT_TRUE(fx.note_workflow.save_called);
    EXPECT_FALSE(fx.reminder_workflow.save_called);
}

TEST(ActionExecutorTests, DeleteReminderActionCallsReminderWorkflow) {
    ExecutorFixture fx;

    EXPECT_TRUE(fx.execute(notes::tui::DeleteSelectedReminderAction{}));
    EXPECT_TRUE(fx.reminder_workflow.delete_called);
    EXPECT_FALSE(fx.note_workflow.delete_called);
}

TEST(ActionExecutorTests, UnhandledDecisionReturnsFalse) {
    ExecutorFixture fx;
    notes::tui::NavigationDecision decision{false, notes::tui::NoAction{}};

    const bool handled = fx.executor.execute(
        decision,
        ftxui::Event::Custom,
        fx.screen,
        fx.state,
        fx.note_workflow,
        fx.reminder_workflow,
        fx.note_sink,
        fx.reminder_sink);

    EXPECT_FALSE(handled);
}

TEST(ActionExecutorTests, PassToNoteMenuActionForwardsToNoteSinkOnly) {
    ExecutorFixture fx;

    EXPECT_TRUE(fx.execute(notes::tui::PassToNoteMenuAction{}));
    EXPECT_EQ(fx.note_sink_calls, 1);
    EXPECT_EQ(fx.reminder_sink_calls, 0);
}

TEST(ActionExecutorTests, SwitchTabActionTogglesTabBetweenZeroAndOne) {
    ExecutorFixture fx;
    fx.state.tab = 0;

    EXPECT_TRUE(fx.execute(notes::tui::SwitchTabAction{}));
    EXPECT_EQ(fx.state.tab, 1);

    EXPECT_TRUE(fx.execute(notes::tui::SwitchTabAction{}));
    EXPECT_EQ(fx.state.tab, 0);
}

TEST(ActionExecutorTests, EnterBrowseModeActionResetsModeToBrowse) {
    ExecutorFixture fx;
    fx.state.mode = notes::tui::Mode::AddNote;

    EXPECT_TRUE(fx.execute(notes::tui::EnterBrowseModeAction{}));
    EXPECT_EQ(fx.state.mode, notes::tui::Mode::Browse);
}

TEST(ActionExecutorTests, BeginAddNoteActionClearsFormFieldsAndSetsMode) {
    ExecutorFixture fx;
    fx.state.form_title = "old";
    fx.state.form_content = "old";
    fx.state.form_date = "2025-01-01";

    EXPECT_TRUE(fx.execute(notes::tui::BeginAddNoteAction{}));
    EXPECT_EQ(fx.state.mode, notes::tui::Mode::AddNote);
    EXPECT_EQ(fx.state.form_title, "");
    EXPECT_EQ(fx.state.form_content, "");
    EXPECT_EQ(fx.state.form_date, "");
}

TEST(ActionExecutorTests, BeginEditNoteActionPopulatesFormFromSelectedNote) {
    ExecutorFixture fx;
    fx.state.notes.push_back(
        notes::Note<notes::IntegerId>::CreateNew(notes::IntegerId{1}, "title-a", "content-a"));
    fx.state.notes.push_back(
        notes::Note<notes::IntegerId>::CreateNew(notes::IntegerId{2}, "title-b", "content-b"));
    fx.state.note_selected = 1;

    EXPECT_TRUE(fx.execute(notes::tui::BeginEditNoteAction{}));
    EXPECT_EQ(fx.state.mode, notes::tui::Mode::EditNote);
    EXPECT_EQ(fx.state.form_title, "title-b");
    EXPECT_EQ(fx.state.form_content, "content-b");
}

TEST(ActionExecutorTests, EnterConfirmDeleteNoteWithEmptyNotesIsNormalizedBackToBrowse) {
    ExecutorFixture fx;
    fx.state.mode = notes::tui::Mode::Browse;

    EXPECT_TRUE(fx.execute(notes::tui::EnterConfirmDeleteNoteAction{}));
    EXPECT_EQ(fx.state.mode, notes::tui::Mode::Browse);
}

TEST(ActionExecutorTests, EnterConfirmDeleteNoteWithNotesKeepsConfirmMode) {
    ExecutorFixture fx;
    fx.state.notes.push_back(
        notes::Note<notes::IntegerId>::CreateNew(notes::IntegerId{1}, "t", "c"));

    EXPECT_TRUE(fx.execute(notes::tui::EnterConfirmDeleteNoteAction{}));
    EXPECT_EQ(fx.state.mode, notes::tui::Mode::ConfirmDeleteNote);
}

} // namespace notes::tests
