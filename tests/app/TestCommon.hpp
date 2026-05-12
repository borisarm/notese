#pragma once

#include <string>
#include <vector>

#include "InMemoryNoteRepository.hpp"
#include "IntegerId.hpp"
#include "Note.hpp"
#include "Reminder.hpp"

namespace notes::tests {

using IntNoteType = notes::Note<notes::IntegerId>;
using IntReminderType = notes::Reminder<notes::IntegerId>;
using IntNoteRepo = InMemoryNoteRepository<IntNoteType>;
using IntReminderRepo = InMemoryNoteRepository<IntReminderType>;

} // namespace notes::tests
