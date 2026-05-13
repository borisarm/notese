#include <gtest/gtest.h>

#include <chrono>

#include "ActionExecutor.hpp"
#include "IntegerId.hpp"
#include "Note.hpp"
#include "Reminder.hpp"

namespace notes::tests {

namespace {

struct NoteWorkflowStub {
    void save_form(notes::tui::TuiState&) {}
    void delete_selected(notes::tui::TuiState&) {}
};

struct ReminderWorkflowStub {
    void save_form(notes::tui::TuiState&) {}
    void delete_selected(notes::tui::TuiState&) {}
};

struct Fixture {
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
        return executor.execute(decision, ftxui::Event::Custom, screen, state,
                                note_workflow, reminder_workflow,
                                note_sink, reminder_sink);
    }
};

} // namespace

TEST(ActionExecutorRemainingVariants, NoActionAndQuitActionAreHandled) {
    Fixture fx;
    EXPECT_TRUE(fx.execute(notes::tui::NoAction{}));
    EXPECT_TRUE(fx.execute(notes::tui::QuitAction{}));
}

TEST(ActionExecutorRemainingVariants, SaveReminderFormActionAndDeleteNoteActionHandled) {
    Fixture fx;
    EXPECT_TRUE(fx.execute(notes::tui::SaveReminderFormAction{}));
    EXPECT_TRUE(fx.execute(notes::tui::DeleteSelectedNoteAction{}));
}

TEST(ActionExecutorRemainingVariants, PassToReminderMenuForwardsToReminderSinkOnly) {
    Fixture fx;
    EXPECT_TRUE(fx.execute(notes::tui::PassToReminderMenuAction{}));
    EXPECT_EQ(fx.reminder_sink_calls, 1);
    EXPECT_EQ(fx.note_sink_calls, 0);
}

TEST(ActionExecutorRemainingVariants, BeginAddReminderClearsFormAndSetsModeAddReminder) {
    Fixture fx;
    fx.state.form.title = "x";
    fx.state.form.date = "2030-01-01";
    EXPECT_TRUE(fx.execute(notes::tui::BeginAddReminderAction{}));
    EXPECT_EQ(fx.state.mode, notes::tui::Mode::AddReminder);
    EXPECT_TRUE(fx.state.form.title.empty());
    EXPECT_TRUE(fx.state.form.date.empty());
}

TEST(ActionExecutorRemainingVariants, BeginEditReminderPopulatesFormFromSelection) {
    Fixture fx;
    auto when = std::chrono::system_clock::now();
    fx.state.reminders.push_back(notes::Reminder<notes::IntegerId>::CreateNew(
        notes::IntegerId{1}, "alpha", "alpha-content", when));
    fx.state.reminders.push_back(notes::Reminder<notes::IntegerId>::CreateNew(
        notes::IntegerId{2}, "beta", "beta-content", when));
    fx.state.selection.reminder_selected = 1;

    EXPECT_TRUE(fx.execute(notes::tui::BeginEditReminderAction{}));
    EXPECT_EQ(fx.state.mode, notes::tui::Mode::EditReminder);
    EXPECT_EQ(fx.state.form.title, "beta");
    EXPECT_EQ(fx.state.form.content, "beta-content");
    EXPECT_FALSE(fx.state.form.date.empty());
}

TEST(ActionExecutorRemainingVariants, BeginEditNoteAndReminderAreNoopsWhenCollectionsEmpty) {
    Fixture fx;
    EXPECT_TRUE(fx.execute(notes::tui::BeginEditNoteAction{}));
    EXPECT_NE(fx.state.mode, notes::tui::Mode::EditNote);
    EXPECT_TRUE(fx.execute(notes::tui::BeginEditReminderAction{}));
    EXPECT_NE(fx.state.mode, notes::tui::Mode::EditReminder);
}

TEST(ActionExecutorRemainingVariants, EnterConfirmDeleteReminderHonoursInvariants) {
    Fixture fx;
    EXPECT_TRUE(fx.execute(notes::tui::EnterConfirmDeleteReminderAction{}));
    EXPECT_EQ(fx.state.mode, notes::tui::Mode::Browse);

    fx.state.reminders.push_back(notes::Reminder<notes::IntegerId>::CreateNew(
        notes::IntegerId{1}, "t", "c", std::chrono::system_clock::now()));
    EXPECT_TRUE(fx.execute(notes::tui::EnterConfirmDeleteReminderAction{}));
    EXPECT_EQ(fx.state.mode, notes::tui::Mode::ConfirmDeleteReminder);
}

} // namespace notes::tests
