#include <gtest/gtest.h>

#include "../../src/app/tui/ViewRouter.hpp"

namespace notes::tests {

TEST(ViewRouterTests, EmptyNoteConfirmDoesNotMutateMode) {
    notes::tui::TuiState state;
    state.mode = notes::tui::Mode::ConfirmDeleteNote;

    std::string title;
    std::string content;
    std::string date;
    std::vector<std::string> entries;
    int selected = 0;

    auto input_title = ftxui::Input(&title, "Title");
    auto input_content = ftxui::Input(&content, "Content");
    auto input_date = ftxui::Input(&date, "Date");
    auto note_menu = ftxui::Menu(&entries, &selected);
    auto reminder_menu = ftxui::Menu(&entries, &selected);

    notes::tui::ViewRouter router;
    (void)router.render(state, input_title, input_content, input_date, note_menu, reminder_menu);

    EXPECT_EQ(state.mode, notes::tui::Mode::ConfirmDeleteNote);
}

TEST(ViewRouterTests, EmptyReminderConfirmDoesNotMutateMode) {
    notes::tui::TuiState state;
    state.mode = notes::tui::Mode::ConfirmDeleteReminder;

    std::string title;
    std::string content;
    std::string date;
    std::vector<std::string> entries;
    int selected = 0;

    auto input_title = ftxui::Input(&title, "Title");
    auto input_content = ftxui::Input(&content, "Content");
    auto input_date = ftxui::Input(&date, "Date");
    auto note_menu = ftxui::Menu(&entries, &selected);
    auto reminder_menu = ftxui::Menu(&entries, &selected);

    notes::tui::ViewRouter router;
    (void)router.render(state, input_title, input_content, input_date, note_menu, reminder_menu);

    EXPECT_EQ(state.mode, notes::tui::Mode::ConfirmDeleteReminder);
}

} // namespace notes::tests
