#include <gtest/gtest.h>

#include <chrono>
#include <optional>
#include <stdexcept>
#include <vector>

#include "FileWriter.hpp"
#include "InMemoryNoteRepository.hpp"
#include "IntegerId.hpp"
#include "Note.hpp"
#include "NoteWorkflowService.hpp"
#include "Reminder.hpp"
#include "ReminderWorkflowService.hpp"
#include "TuiState.hpp"

namespace notes::tests {

namespace {

using NoteT = notes::Note<notes::IntegerId>;
using ReminderT = notes::Reminder<notes::IntegerId>;
using NoteRepo = InMemoryNoteRepository<NoteT>;
using ReminderRepo = InMemoryNoteRepository<ReminderT>;

// Repo that wraps an InMemoryNoteRepository and throws FileWriteError on save/remove.
template <typename T>
class ThrowingRepo {
public:
    using Id = typename T::IdType;

    void save(const T&) {
        throw notes::infra::FileWriteError("/tmp/forced");
    }
    void remove(const Id&) {
        throw notes::infra::FileWriteError("/tmp/forced");
    }
    std::optional<T> get(const Id& id) const { return inner_.get(id); }
    std::vector<T> list() const { return inner_.list(); }
    Id next_id() { return inner_.next_id(); }

    void seed(const T& t) { inner_.save(t); }

private:
    InMemoryNoteRepository<T> inner_;
};

} // namespace

TEST(NoteWorkflowServiceMore, SaveFormInEditModeUpdatesSelectedNote) {
    NoteRepo repo;
    repo.save(NoteT::CreateNew(notes::IntegerId{1}, "old", "old-content"));

    notes::tui::NoteWorkflowService<NoteRepo> service(repo);
    notes::tui::TuiState state;
    service.refresh(state);

    state.mode = notes::tui::Mode::EditNote;
    state.selection.note_selected = 0;
    state.form.title = "new";
    state.form.content = "new-content";
    service.save_form(state);

    ASSERT_EQ(state.notes.size(), 1u);
    EXPECT_EQ(state.notes[0].title(), "new");
    EXPECT_EQ(state.notes[0].content(), "new-content");
    EXPECT_EQ(state.mode, notes::tui::Mode::Browse);
}

TEST(NoteWorkflowServiceMore, RefreshSortsNotesAndClampsOutOfRangeSelection) {
    NoteRepo repo;
    repo.save(NoteT::CreateNew(notes::IntegerId{1}, "z-title", ""));
    repo.save(NoteT::CreateNew(notes::IntegerId{2}, "a-title", ""));

    notes::tui::NoteWorkflowService<NoteRepo> service(repo);
    notes::tui::TuiState state;
    state.selection.note_selected = 50;
    service.refresh(state);

    ASSERT_EQ(state.notes.size(), 2u);
    EXPECT_EQ(state.notes[0].title(), "a-title");
    EXPECT_EQ(state.notes[1].title(), "z-title");
    EXPECT_EQ(state.selection.note_selected, 1);
}

TEST(NoteWorkflowServiceMore, SaveFormFailureCapturesStatusMessage) {
    ThrowingRepo<NoteT> repo;
    notes::tui::NoteWorkflowService<ThrowingRepo<NoteT>> service(repo);
    notes::tui::TuiState state;

    state.mode = notes::tui::Mode::AddNote;
    state.form.title = "title";
    state.form.content = "body";
    service.save_form(state);

    EXPECT_FALSE(state.status_message.empty());
}

TEST(NoteWorkflowServiceMore, DeleteSelectedFailureCapturesStatusMessage) {
    ThrowingRepo<NoteT> repo;
    repo.seed(NoteT::CreateNew(notes::IntegerId{1}, "title", "body"));
    notes::tui::NoteWorkflowService<ThrowingRepo<NoteT>> service(repo);
    notes::tui::TuiState state;
    service.refresh(state);

    service.delete_selected(state);
    EXPECT_FALSE(state.status_message.empty());
}

TEST(NoteWorkflowServiceMore, DeleteSelectedWithEmptyCollectionDoesNotThrow) {
    NoteRepo repo;
    notes::tui::NoteWorkflowService<NoteRepo> service(repo);
    notes::tui::TuiState state;
    EXPECT_NO_THROW(service.delete_selected(state));
    EXPECT_EQ(state.mode, notes::tui::Mode::Browse);
}

TEST(NoteWorkflowServiceMore, SaveFormInEditModeWithEmptyCollectionFallsThrough) {
    NoteRepo repo;
    notes::tui::NoteWorkflowService<NoteRepo> service(repo);
    notes::tui::TuiState state;
    state.mode = notes::tui::Mode::EditNote;
    state.form.title = "ignored";
    state.form.content = "ignored";
    EXPECT_NO_THROW(service.save_form(state));
    EXPECT_EQ(state.mode, notes::tui::Mode::Browse);
}

TEST(ReminderWorkflowServiceMore, SaveFormInEditModeUpdatesSelectedReminder) {
    ReminderRepo repo;
    repo.save(ReminderT::CreateNew(notes::IntegerId{1}, "old", "old-content",
                                   std::chrono::system_clock::now()));

    notes::tui::ReminderWorkflowService<ReminderRepo> service(repo);
    notes::tui::TuiState state;
    service.refresh(state);

    state.mode = notes::tui::Mode::EditReminder;
    state.selection.reminder_selected = 0;
    state.form.title = "new";
    state.form.content = "new-content";
    state.form.date = "2031-06-14";
    service.save_form(state);

    ASSERT_EQ(state.reminders.size(), 1u);
    EXPECT_EQ(state.reminders[0].title(), "new");
    EXPECT_EQ(state.reminders[0].content(), "new-content");
    EXPECT_EQ(state.mode, notes::tui::Mode::Browse);
}

TEST(ReminderWorkflowServiceMore, RefreshSortsBySoonestRemindAtAndClampsSelection) {
    ReminderRepo repo;
    auto now = std::chrono::system_clock::now();
    repo.save(ReminderT::CreateNew(notes::IntegerId{1}, "later", "",
                                   now + std::chrono::hours(48)));
    repo.save(ReminderT::CreateNew(notes::IntegerId{2}, "sooner", "",
                                   now + std::chrono::hours(1)));

    notes::tui::ReminderWorkflowService<ReminderRepo> service(repo);
    notes::tui::TuiState state;
    state.selection.reminder_selected = 99;
    service.refresh(state);

    ASSERT_EQ(state.reminders.size(), 2u);
    EXPECT_EQ(state.reminders[0].title(), "sooner");
    EXPECT_EQ(state.reminders[1].title(), "later");
    EXPECT_EQ(state.selection.reminder_selected, 1);
}

TEST(ReminderWorkflowServiceMore, SaveFormFailureCapturesStatusMessage) {
    ThrowingRepo<ReminderT> repo;
    notes::tui::ReminderWorkflowService<ThrowingRepo<ReminderT>> service(repo);
    notes::tui::TuiState state;
    state.mode = notes::tui::Mode::AddReminder;
    state.form.title = "title";
    state.form.content = "body";
    state.form.date = "2030-05-13";
    service.save_form(state);
    EXPECT_FALSE(state.status_message.empty());
}

TEST(ReminderWorkflowServiceMore, DeleteSelectedFailureCapturesStatusMessage) {
    ThrowingRepo<ReminderT> repo;
    repo.seed(ReminderT::CreateNew(notes::IntegerId{1}, "title", "body",
                                   std::chrono::system_clock::now()));
    notes::tui::ReminderWorkflowService<ThrowingRepo<ReminderT>> service(repo);
    notes::tui::TuiState state;
    service.refresh(state);

    service.delete_selected(state);
    EXPECT_FALSE(state.status_message.empty());
}

TEST(ReminderWorkflowServiceMore, DeleteSelectedWithEmptyCollectionDoesNotThrow) {
    ReminderRepo repo;
    notes::tui::ReminderWorkflowService<ReminderRepo> service(repo);
    notes::tui::TuiState state;
    EXPECT_NO_THROW(service.delete_selected(state));
    EXPECT_EQ(state.mode, notes::tui::Mode::Browse);
}

TEST(ReminderWorkflowServiceMore, SaveFormInEditModeWithEmptyCollectionFallsThrough) {
    ReminderRepo repo;
    notes::tui::ReminderWorkflowService<ReminderRepo> service(repo);
    notes::tui::TuiState state;
    state.mode = notes::tui::Mode::EditReminder;
    state.form.title = "ignored";
    state.form.content = "ignored";
    state.form.date = "2030-01-01";
    EXPECT_NO_THROW(service.save_form(state));
    EXPECT_EQ(state.mode, notes::tui::Mode::Browse);
}

} // namespace notes::tests
