#pragma once

#include <algorithm>
#include "NoteRepositoryConcept.hpp"
#include "TuiState.hpp"

namespace notes::tui {

template <NoteRepositoryConcept<NoteType> NoteRepo>
class NoteWorkflowService {
public:
    explicit NoteWorkflowService(NoteRepo& note_repo)
        : note_repo_(note_repo) {}

    void refresh(TuiState& state) {
        state.notes = note_repo_.list();
        std::sort(state.notes.begin(), state.notes.end(), [](const NoteType& a, const NoteType& b) {
            return a.title() < b.title();
        });

        state.note_entries.clear();
        for (const auto& note : state.notes) {
            state.note_entries.push_back(note.id().to_string() + "  " + note.title());
        }

        if (state.note_selected >= (int)state.notes.size()) {
            state.note_selected = std::max(0, (int)state.notes.size() - 1);
        }
    }

    void save_form(TuiState& state) {
        if (state.mode == Mode::AddNote) {
            auto id = note_repo_.next_id();
            auto note = NoteType::CreateNew(id, state.form_title, state.form_content);
            note_repo_.save(note);
        } else if (!state.notes.empty()) {
            int idx = std::clamp(state.note_selected, 0, (int)state.notes.size() - 1);
            auto updated = state.notes[idx]
                .with_updated_title(state.form_title)
                .with_updated_content(state.form_content);
            note_repo_.save(updated);
        }

        refresh(state);
        state.mode = Mode::Browse;
    }

    void delete_selected(TuiState& state) {
        if (!state.notes.empty()) {
            int idx = std::clamp(state.note_selected, 0, (int)state.notes.size() - 1);
            note_repo_.remove(state.notes[idx].id());
        }

        refresh(state);
        state.mode = Mode::Browse;
    }

private:
    NoteRepo& note_repo_;
};

} // namespace notes::tui