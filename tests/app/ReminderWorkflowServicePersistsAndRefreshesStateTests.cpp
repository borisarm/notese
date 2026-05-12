#include <gtest/gtest.h>

#include <chrono>
#include "ReminderWorkflowService.hpp"
#include "TuiState.hpp"
#include "IntegerId.hpp"
#include "Reminder.hpp"
#include "InMemoryNoteRepository.hpp"

namespace notes::tests {

using IntReminderType = notes::Reminder<notes::IntegerId>;
using IntReminderRepo = InMemoryNoteRepository<IntReminderType>;

TEST(ReminderWorkflowServiceTests, SaveFormInAddModePersistsAndReturnsToBrowse) {
    IntReminderRepo repo;
    notes::tui::ReminderWorkflowService<IntReminderRepo> service(repo);
    notes::tui::TuiState state;

    state.mode = notes::tui::Mode::AddReminder;
    state.form.title = "Pay rent";
    state.form.content = "Tomorrow";
    state.form.date = "2026-05-20";

    service.save_form(state);

    EXPECT_EQ(state.mode, notes::tui::Mode::Browse);
    ASSERT_EQ(state.reminders.size(), 1u);
    EXPECT_EQ(state.reminders[0].title(), "Pay rent");
    ASSERT_EQ(state.reminder_entries.size(), 1u);
}

TEST(ReminderWorkflowServiceTests, DeleteSelectedRemovesCurrentReminder) {
    IntReminderRepo repo;
    notes::tui::ReminderWorkflowService<IntReminderRepo> service(repo);
    notes::tui::TuiState state;

    auto ts = std::chrono::system_clock::now();
    repo.save(IntReminderType::CreateNew(notes::IntegerId{1}, "A", "A", ts));
    repo.save(IntReminderType::CreateNew(notes::IntegerId{2}, "B", "B", ts));

    service.refresh(state);
    state.selection.reminder_selected = 0;
    state.mode = notes::tui::Mode::ConfirmDeleteReminder;

    service.delete_selected(state);

    EXPECT_EQ(state.mode, notes::tui::Mode::Browse);
    ASSERT_EQ(state.reminders.size(), 1u);
}

} // namespace notes::tests
