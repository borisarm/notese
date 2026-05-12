#pragma once
#include "TuiTypes.hpp"
#include "TuiController.hpp"

namespace notes::tui {

template <NoteRepositoryConcept<NoteType> NoteRepo, NoteRepositoryConcept<ReminderType> ReminderRepo>
int run(NoteRepo& note_repo, ReminderRepo& reminder_repo) {
    TuiController<NoteRepo, ReminderRepo> controller(note_repo, reminder_repo);
    return controller.run();
}

} // namespace notes::tui
