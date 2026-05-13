#include <gtest/gtest.h>

#include <variant>

#include "IntegerId.hpp"
#include "NavigationService.hpp"
#include "Note.hpp"
#include "Reminder.hpp"
#include "TuiState.hpp"

namespace notes::tests {

namespace {

notes::Note<notes::IntegerId> sample_note() {
    return notes::Note<notes::IntegerId>::CreateNew(notes::IntegerId{1}, "t", "c");
}

notes::Reminder<notes::IntegerId> sample_reminder() {
    return notes::Reminder<notes::IntegerId>::CreateNew(
        notes::IntegerId{1}, "t", "c", std::chrono::system_clock::now());
}

notes::tui::NavigationDecision dispatch(notes::tui::Mode mode,
                                        const ftxui::Event& event,
                                        bool content_focused = false,
                                        int tab = 0,
                                        bool with_notes = false,
                                        bool with_reminders = false,
                                        const std::string& title = "",
                                        const std::string& date = "") {
    notes::tui::NavigationService service;
    notes::tui::TuiState state;
    state.mode = mode;
    state.selection.tab = tab;
    state.form.title = title;
    state.form.date = date;
    if (with_notes) state.notes.push_back(sample_note());
    if (with_reminders) state.reminders.push_back(sample_reminder());
    return service.handle_event(event, state, content_focused);
}

} // namespace

TEST(NavigationServiceBranches, NoteFormEscapeReturnsEnterBrowseMode) {
    const auto d = dispatch(notes::tui::Mode::AddNote, ftxui::Event::Escape);
    EXPECT_TRUE(d.handled);
    EXPECT_TRUE(std::holds_alternative<notes::tui::EnterBrowseModeAction>(d.action));
}

TEST(NavigationServiceBranches, NoteFormNonReturnEventIsUnhandled) {
    const auto d = dispatch(notes::tui::Mode::EditNote, ftxui::Event::Character('x'));
    EXPECT_FALSE(d.handled);
}

TEST(NavigationServiceBranches, NoteFormReturnWithContentFocusedIsUnhandled) {
    const auto d = dispatch(notes::tui::Mode::AddNote, ftxui::Event::Return, true,
                            0, false, false, "title");
    EXPECT_FALSE(d.handled);
}

TEST(NavigationServiceBranches, NoteFormReturnWithEmptyTitleProducesNoActionHandled) {
    const auto d = dispatch(notes::tui::Mode::AddNote, ftxui::Event::Return);
    EXPECT_TRUE(d.handled);
    EXPECT_TRUE(std::holds_alternative<notes::tui::NoAction>(d.action));
}

TEST(NavigationServiceBranches, ReminderFormEscapeReturnsEnterBrowseMode) {
    const auto d = dispatch(notes::tui::Mode::AddReminder, ftxui::Event::Escape);
    EXPECT_TRUE(std::holds_alternative<notes::tui::EnterBrowseModeAction>(d.action));
}

TEST(NavigationServiceBranches, ReminderFormReturnRequiresBothTitleAndDate) {
    EXPECT_TRUE(std::holds_alternative<notes::tui::NoAction>(
        dispatch(notes::tui::Mode::AddReminder, ftxui::Event::Return,
                 false, 0, false, false, "", "2030-05-13").action));
    EXPECT_TRUE(std::holds_alternative<notes::tui::NoAction>(
        dispatch(notes::tui::Mode::AddReminder, ftxui::Event::Return,
                 false, 0, false, false, "title", "").action));
    EXPECT_TRUE(std::holds_alternative<notes::tui::SaveReminderFormAction>(
        dispatch(notes::tui::Mode::AddReminder, ftxui::Event::Return,
                 false, 0, false, false, "title", "2030-05-13").action));
}

TEST(NavigationServiceBranches, ReminderFormReturnContentFocusedIsUnhandled) {
    const auto d = dispatch(notes::tui::Mode::EditReminder, ftxui::Event::Return, true,
                            0, false, false, "title", "2030-05-13");
    EXPECT_FALSE(d.handled);
}

TEST(NavigationServiceBranches, ReminderFormUnknownKeyIsUnhandled) {
    const auto d = dispatch(notes::tui::Mode::EditReminder, ftxui::Event::Character('x'));
    EXPECT_FALSE(d.handled);
}

TEST(NavigationServiceBranches, NoteConfirmDeleteHandlesYesNoEscapeAndOther) {
    EXPECT_TRUE(std::holds_alternative<notes::tui::DeleteSelectedNoteAction>(
        dispatch(notes::tui::Mode::ConfirmDeleteNote, ftxui::Event::Character('y')).action));
    EXPECT_TRUE(std::holds_alternative<notes::tui::EnterBrowseModeAction>(
        dispatch(notes::tui::Mode::ConfirmDeleteNote, ftxui::Event::Character('n')).action));
    EXPECT_TRUE(std::holds_alternative<notes::tui::EnterBrowseModeAction>(
        dispatch(notes::tui::Mode::ConfirmDeleteNote, ftxui::Event::Escape).action));
    EXPECT_TRUE(std::holds_alternative<notes::tui::NoAction>(
        dispatch(notes::tui::Mode::ConfirmDeleteNote, ftxui::Event::Character('z')).action));
}

TEST(NavigationServiceBranches, ReminderConfirmDeleteHandlesYesNoEscapeAndOther) {
    EXPECT_TRUE(std::holds_alternative<notes::tui::DeleteSelectedReminderAction>(
        dispatch(notes::tui::Mode::ConfirmDeleteReminder, ftxui::Event::Character('y')).action));
    EXPECT_TRUE(std::holds_alternative<notes::tui::EnterBrowseModeAction>(
        dispatch(notes::tui::Mode::ConfirmDeleteReminder, ftxui::Event::Character('n')).action));
    EXPECT_TRUE(std::holds_alternative<notes::tui::EnterBrowseModeAction>(
        dispatch(notes::tui::Mode::ConfirmDeleteReminder, ftxui::Event::Escape).action));
    EXPECT_TRUE(std::holds_alternative<notes::tui::NoAction>(
        dispatch(notes::tui::Mode::ConfirmDeleteReminder, ftxui::Event::Character('z')).action));
}

TEST(NavigationServiceBranches, BrowseQuitAndEscapeMapToQuit) {
    EXPECT_TRUE(std::holds_alternative<notes::tui::QuitAction>(
        dispatch(notes::tui::Mode::Browse, ftxui::Event::Character('q')).action));
    EXPECT_TRUE(std::holds_alternative<notes::tui::QuitAction>(
        dispatch(notes::tui::Mode::Browse, ftxui::Event::Escape).action));
}

TEST(NavigationServiceBranches, BrowseAddDispatchesByTab) {
    EXPECT_TRUE(std::holds_alternative<notes::tui::BeginAddNoteAction>(
        dispatch(notes::tui::Mode::Browse, ftxui::Event::Character('a'), false, 0).action));
    EXPECT_TRUE(std::holds_alternative<notes::tui::BeginAddReminderAction>(
        dispatch(notes::tui::Mode::Browse, ftxui::Event::Character('a'), false, 1).action));
}

TEST(NavigationServiceBranches, BrowseEditRequiresEntitiesAndDispatchesByTab) {
    EXPECT_TRUE(std::holds_alternative<notes::tui::BeginEditNoteAction>(
        dispatch(notes::tui::Mode::Browse, ftxui::Event::Character('e'), false, 0, true).action));
    EXPECT_TRUE(std::holds_alternative<notes::tui::BeginEditReminderAction>(
        dispatch(notes::tui::Mode::Browse, ftxui::Event::Character('e'), false, 1, false, true).action));
    EXPECT_TRUE(std::holds_alternative<notes::tui::NoAction>(
        dispatch(notes::tui::Mode::Browse, ftxui::Event::Character('e'), false, 0).action));
    EXPECT_TRUE(std::holds_alternative<notes::tui::NoAction>(
        dispatch(notes::tui::Mode::Browse, ftxui::Event::Character('e'), false, 1).action));
}

TEST(NavigationServiceBranches, BrowseDeleteRequiresEntitiesAndDispatchesByTab) {
    EXPECT_TRUE(std::holds_alternative<notes::tui::EnterConfirmDeleteNoteAction>(
        dispatch(notes::tui::Mode::Browse, ftxui::Event::Character('d'), false, 0, true).action));
    EXPECT_TRUE(std::holds_alternative<notes::tui::EnterConfirmDeleteReminderAction>(
        dispatch(notes::tui::Mode::Browse, ftxui::Event::Character('d'), false, 1, false, true).action));
    EXPECT_TRUE(std::holds_alternative<notes::tui::NoAction>(
        dispatch(notes::tui::Mode::Browse, ftxui::Event::Character('d'), false, 0).action));
    EXPECT_TRUE(std::holds_alternative<notes::tui::NoAction>(
        dispatch(notes::tui::Mode::Browse, ftxui::Event::Character('d'), false, 1).action));
}

TEST(NavigationServiceBranches, BrowseDefaultEventDispatchesToActiveTabMenu) {
    EXPECT_TRUE(std::holds_alternative<notes::tui::PassToNoteMenuAction>(
        dispatch(notes::tui::Mode::Browse, ftxui::Event::ArrowDown, false, 0).action));
    EXPECT_TRUE(std::holds_alternative<notes::tui::PassToReminderMenuAction>(
        dispatch(notes::tui::Mode::Browse, ftxui::Event::ArrowDown, false, 1).action));
}

} // namespace notes::tests
