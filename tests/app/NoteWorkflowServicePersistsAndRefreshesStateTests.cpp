#include <gtest/gtest.h>

#include "NoteWorkflowService.hpp"
#include "TuiState.hpp"
#include "IntegerId.hpp"
#include "Note.hpp"
#include "InMemoryNoteRepository.hpp"

namespace notes::tests {

using IntNoteType = notes::Note<notes::IntegerId>;
using IntNoteRepo = InMemoryNoteRepository<IntNoteType>;

TEST(NoteWorkflowServiceTests, SaveFormInAddModePersistsAndReturnsToBrowse) {
    IntNoteRepo repo;
    notes::tui::NoteWorkflowService<IntNoteRepo> service(repo);
    notes::tui::TuiState state;

    state.mode = notes::tui::Mode::AddNote;
    state.form_title = "Title";
    state.form_content = "Body";

    service.save_form(state);

    EXPECT_EQ(state.mode, notes::tui::Mode::Browse);
    ASSERT_EQ(state.notes.size(), 1u);
    EXPECT_EQ(state.notes[0].title(), "Title");
    EXPECT_EQ(state.notes[0].content(), "Body");
    ASSERT_EQ(state.note_entries.size(), 1u);
}

TEST(NoteWorkflowServiceTests, DeleteSelectedRemovesCurrentNote) {
    IntNoteRepo repo;
    notes::tui::NoteWorkflowService<IntNoteRepo> service(repo);
    notes::tui::TuiState state;

    repo.save(IntNoteType::CreateNew(notes::IntegerId{1}, "A", "A"));
    repo.save(IntNoteType::CreateNew(notes::IntegerId{2}, "B", "B"));

    service.refresh(state);
    state.note_selected = 0;
    state.mode = notes::tui::Mode::ConfirmDeleteNote;

    service.delete_selected(state);

    EXPECT_EQ(state.mode, notes::tui::Mode::Browse);
    ASSERT_EQ(state.notes.size(), 1u);
}

} // namespace notes::tests
