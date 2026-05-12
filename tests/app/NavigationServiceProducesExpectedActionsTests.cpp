#include <gtest/gtest.h>

#include <variant>
#include "NavigationService.hpp"
#include "TuiState.hpp"
#include "IntegerId.hpp"
#include "Note.hpp"

namespace notes::tests {

TEST(NavigationServiceTests, ReturnOnAddNoteProducesSaveNoteFormAction) {
    notes::tui::NavigationService service;
    notes::tui::TuiState state;
    state.mode = notes::tui::Mode::AddNote;
    state.form_title = "title";

    const auto decision = service.handle_event(ftxui::Event::Return, state, false);

    EXPECT_TRUE(decision.handled);
    EXPECT_TRUE(std::holds_alternative<notes::tui::SaveNoteFormAction>(decision.action));
}

TEST(NavigationServiceTests, TabInBrowseProducesSwitchTabActionWithoutMutatingState) {
    notes::tui::NavigationService service;
    notes::tui::TuiState state;
    state.mode = notes::tui::Mode::Browse;
    state.tab = 0;

    const auto decision = service.handle_event(ftxui::Event::Tab, state, false);

    EXPECT_TRUE(decision.handled);
    EXPECT_EQ(state.tab, 0);
    EXPECT_TRUE(std::holds_alternative<notes::tui::SwitchTabAction>(decision.action));
}

TEST(NavigationServiceTests, DeleteInBrowseWithNotesProducesEnterConfirmDeleteNoteActionWithoutMutatingState) {
    notes::tui::NavigationService service;
    notes::tui::TuiState state;
    state.mode = notes::tui::Mode::Browse;
    state.tab = 0;
    state.notes.push_back(notes::Note<notes::IntegerId>::CreateNew(notes::IntegerId{1}, "t", "c"));

    const auto decision = service.handle_event(ftxui::Event::Character('d'), state, false);

    EXPECT_TRUE(decision.handled);
    EXPECT_EQ(state.mode, notes::tui::Mode::Browse);
    EXPECT_TRUE(std::holds_alternative<notes::tui::EnterConfirmDeleteNoteAction>(decision.action));
}

} // namespace notes::tests
