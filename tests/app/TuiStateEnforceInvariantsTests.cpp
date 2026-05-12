#include <gtest/gtest.h>

#include "../../src/app/tui/TuiState.hpp"
#include "../../src/domain/include/IntegerId.hpp"
#include "../../src/domain/include/Note.hpp"

namespace notes::tests {

TEST(TuiStateInvariantsTests, ConfirmDeleteNoteWithEmptyNotesFallsBackToBrowse) {
    notes::tui::TuiState state;
    state.mode = notes::tui::Mode::ConfirmDeleteNote;

    state.enforce_invariants();

    EXPECT_EQ(state.mode, notes::tui::Mode::Browse);
}

TEST(TuiStateInvariantsTests, ConfirmDeleteReminderWithEmptyRemindersFallsBackToBrowse) {
    notes::tui::TuiState state;
    state.mode = notes::tui::Mode::ConfirmDeleteReminder;

    state.enforce_invariants();

    EXPECT_EQ(state.mode, notes::tui::Mode::Browse);
}

TEST(TuiStateInvariantsTests, ConfirmDeleteNoteWithNotesPreservesMode) {
    notes::tui::TuiState state;
    state.mode = notes::tui::Mode::ConfirmDeleteNote;
    state.notes.push_back(notes::Note<notes::IntegerId>::CreateNew(notes::IntegerId{1}, "t", "c"));

    state.enforce_invariants();

    EXPECT_EQ(state.mode, notes::tui::Mode::ConfirmDeleteNote);
}

TEST(TuiStateInvariantsTests, BrowseModeIsNeverChanged) {
    notes::tui::TuiState state;
    state.mode = notes::tui::Mode::Browse;

    state.enforce_invariants();

    EXPECT_EQ(state.mode, notes::tui::Mode::Browse);
}

} // namespace notes::tests
