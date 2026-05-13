#include <gtest/gtest.h>

#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>

#include <chrono>
#include <string>
#include <vector>

#include "BrowseView.hpp"
#include "ConfirmView.hpp"
#include "DateInputParser.hpp"
#include "FormViews.hpp"
#include "IntegerId.hpp"
#include "MarkdownPreview.hpp"
#include "Note.hpp"
#include "Reminder.hpp"
#include "TuiState.hpp"
#include "TuiTypes.hpp"
#include "ViewRouter.hpp"
#include "VisualComponents.hpp"

namespace notes::tests {

namespace {

ftxui::Screen make_screen() {
    return ftxui::Screen::Create(ftxui::Dimension::Fixed(120), ftxui::Dimension::Fixed(40));
}

void render(ftxui::Element element) {
    auto screen = make_screen();
    ftxui::Render(screen, element);
    // Touch the rendered output so the call isn't dead-stripped.
    EXPECT_FALSE(screen.ToString().empty());
}

notes::tui::TuiState make_browse_state() {
    notes::tui::TuiState state;
    state.notes.push_back(notes::Note<notes::IntegerId>::CreateNew(
        notes::IntegerId{1}, "first note", "# heading\nbody line"));
    state.notes.push_back(notes::Note<notes::IntegerId>::CreateNew(
        notes::IntegerId{2}, "second note", ""));
    state.reminders.push_back(notes::Reminder<notes::IntegerId>::CreateNew(
        notes::IntegerId{1}, "first reminder", "* item one\n* item two",
        std::chrono::system_clock::now()));
    state.note_entries = {"1  first note", "2  second note"};
    state.reminder_entries = {"1  first reminder"};
    return state;
}

struct MenuComponents {
    notes::tui::TuiState* state_ptr;
    ftxui::Component note_menu;
    ftxui::Component reminder_menu;
    ftxui::Component input_title;
    ftxui::Component input_content;
    ftxui::Component input_date;

    explicit MenuComponents(notes::tui::TuiState& state)
        : state_ptr(&state),
          note_menu(ftxui::Menu(&state.note_entries, &state.selection.note_selected)),
          reminder_menu(ftxui::Menu(&state.reminder_entries, &state.selection.reminder_selected)),
          input_title(ftxui::Input(&state.form.title, "Title")),
          input_content(ftxui::Input(&state.form.content, "Content")),
          input_date(ftxui::Input(&state.form.date, "YYYY-MM-DD")) {}
};

} // namespace

TEST(MarkdownPreviewTests, EmptyInputProducesEmptyPlaceholder) {
    render(notes::tui::render_markdown_preview(""));
}

TEST(MarkdownPreviewTests, RendersHeadingsListsBlockquotesRulesAndCodeFences) {
    const std::string md =
        "# h1\n"
        "## h2\n"
        "### h3\n"
        "#### h4\n"
        "##### h5\n"
        "###### h6\n"
        "\n"
        "paragraph text\n"
        "> blockquote line\n"
        "---\n"
        "***\n"
        "- bullet one\n"
        "* bullet two\n"
        "+ bullet three\n"
        "1. ordered first\n"
        "2. ordered second\n"
        "```\n"
        "code line one\n"
        "code line two\n"
        "```\n"
        "after code\n";
    render(notes::tui::render_markdown_preview(md));
}

TEST(MarkdownPreviewTests, IsOrderedListItemRecognizesDigitsFollowedByDotSpace) {
    EXPECT_TRUE(notes::tui::is_ordered_list_item("1. item"));
    EXPECT_TRUE(notes::tui::is_ordered_list_item("42. item"));
    EXPECT_FALSE(notes::tui::is_ordered_list_item("a. item"));
    EXPECT_FALSE(notes::tui::is_ordered_list_item("1 item"));
    EXPECT_FALSE(notes::tui::is_ordered_list_item("1."));
}

TEST(VisualComponentsTests, StatusBarRendersBothEmptyAndNonEmptyMessages) {
    render(notes::tui::visual::status_bar(""));
    render(notes::tui::visual::status_bar("an error"));
    render(notes::tui::visual::app_title());
    render(notes::tui::visual::help_footer());
    render(notes::tui::visual::empty_state("nothing", "hint"));
}

TEST(BrowseViewTests, RendersNoteTabWithNotesAndWithoutNotes) {
    auto state = make_browse_state();
    state.selection.tab = 0;
    MenuComponents components{state};
    render(notes::tui::BrowseView::render(state, components.note_menu, components.reminder_menu));

    state.notes.clear();
    state.note_entries.clear();
    render(notes::tui::BrowseView::render(state, components.note_menu, components.reminder_menu));
}

TEST(BrowseViewTests, RendersReminderTabWithRemindersAndWithoutReminders) {
    auto state = make_browse_state();
    state.selection.tab = 1;
    MenuComponents components{state};
    render(notes::tui::BrowseView::render(state, components.note_menu, components.reminder_menu));

    state.reminders.clear();
    state.reminder_entries.clear();
    render(notes::tui::BrowseView::render(state, components.note_menu, components.reminder_menu));
}

TEST(BrowseViewTests, ClampsOutOfRangeSelectionWhenRendering) {
    auto state = make_browse_state();
    state.selection.note_selected = 999;
    state.selection.reminder_selected = 999;
    MenuComponents components{state};
    state.selection.tab = 0;
    render(notes::tui::BrowseView::render(state, components.note_menu, components.reminder_menu));
    state.selection.tab = 1;
    render(notes::tui::BrowseView::render(state, components.note_menu, components.reminder_menu));
}

TEST(FormViewsTests, RendersAddNoteAndEditNoteForms) {
    notes::tui::TuiState state;
    state.mode = notes::tui::Mode::AddNote;
    MenuComponents components{state};
    render(notes::tui::FormViews::render_note_form(state, components.input_title, components.input_content));

    state.mode = notes::tui::Mode::EditNote;
    state.form.title = "title";
    state.form.content = "content";
    render(notes::tui::FormViews::render_note_form(state, components.input_title, components.input_content));
}

TEST(FormViewsTests, RendersAddReminderAndEditReminderForms) {
    notes::tui::TuiState state;
    state.mode = notes::tui::Mode::AddReminder;
    MenuComponents components{state};
    render(notes::tui::FormViews::render_reminder_form(
        state, components.input_title, components.input_content, components.input_date));

    state.mode = notes::tui::Mode::EditReminder;
    state.form.title = "title";
    state.form.content = "content";
    state.form.date = "2030-05-13";
    render(notes::tui::FormViews::render_reminder_form(
        state, components.input_title, components.input_content, components.input_date));
}

TEST(ConfirmViewTests, RendersBothDeleteConfirmationsWhenItemsExistAndNoOpsWhenEmpty) {
    auto state = make_browse_state();
    render(notes::tui::ConfirmView::render_note_delete(state));
    render(notes::tui::ConfirmView::render_reminder_delete(state));

    state.notes.clear();
    state.reminders.clear();
    render(notes::tui::ConfirmView::render_note_delete(state));
    render(notes::tui::ConfirmView::render_reminder_delete(state));
}

TEST(ViewRouterTests, RendersAllModesIncludingFormsConfirmationsAndBrowse) {
    auto state = make_browse_state();
    MenuComponents components{state};
    notes::tui::ViewRouter router;

    state.mode = notes::tui::Mode::Browse;
    render(router.render(state, components.input_title, components.input_content,
                         components.input_date, components.note_menu, components.reminder_menu));

    state.mode = notes::tui::Mode::AddNote;
    render(router.render(state, components.input_title, components.input_content,
                         components.input_date, components.note_menu, components.reminder_menu));

    state.mode = notes::tui::Mode::EditNote;
    render(router.render(state, components.input_title, components.input_content,
                         components.input_date, components.note_menu, components.reminder_menu));

    state.mode = notes::tui::Mode::AddReminder;
    render(router.render(state, components.input_title, components.input_content,
                         components.input_date, components.note_menu, components.reminder_menu));

    state.mode = notes::tui::Mode::EditReminder;
    render(router.render(state, components.input_title, components.input_content,
                         components.input_date, components.note_menu, components.reminder_menu));

    state.mode = notes::tui::Mode::ConfirmDeleteNote;
    render(router.render(state, components.input_title, components.input_content,
                         components.input_date, components.note_menu, components.reminder_menu));

    state.mode = notes::tui::Mode::ConfirmDeleteReminder;
    render(router.render(state, components.input_title, components.input_content,
                         components.input_date, components.note_menu, components.reminder_menu));

    auto empty_state_value = notes::tui::TuiState{};
    state = empty_state_value;
    state.mode = notes::tui::Mode::ConfirmDeleteNote;
    render(router.render(state, components.input_title, components.input_content,
                         components.input_date, components.note_menu, components.reminder_menu));
    state.mode = notes::tui::Mode::ConfirmDeleteReminder;
    render(router.render(state, components.input_title, components.input_content,
                         components.input_date, components.note_menu, components.reminder_menu));
}

TEST(DateInputParserTests, ValidDateProducesNonZeroTimePoint) {
    auto tp = notes::tui::parse_date("2030-05-13");
    EXPECT_NE(tp, std::chrono::system_clock::time_point{});
}

TEST(DateInputParserTests, InvalidDateProducesZeroTimePoint) {
    auto tp = notes::tui::parse_date("not-a-date");
    EXPECT_EQ(tp, std::chrono::system_clock::time_point{});
}

} // namespace notes::tests
