#pragma once

#include "Note.hpp"
#include "Reminder.hpp"
#include "IntegerId.hpp"

namespace notes::tui {

using NoteType = Note<IntegerId>;
using ReminderType = Reminder<IntegerId>;

enum class Mode { Browse, AddNote, EditNote, ConfirmDeleteNote,
                  AddReminder, EditReminder, ConfirmDeleteReminder };

} // namespace notes::tui