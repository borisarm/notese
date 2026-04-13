#include "InMemoryNoteRepository.hpp"
#include "NoteRepositoryConcept.hpp"
#include "Note.hpp"
#include "Reminder.hpp"
#include "StringId.hpp"
#include "IntegerId.hpp"
#include "MarkdownNoteRepository.hpp"
#include "MarkdownReminderRepository.hpp"

using namespace notes;

using StringNote = Note<StringId>;
using StringRepo = notes::MarkdownNoteRepository<StringNote>;

static_assert(NoteRepositoryConcept<InMemoryNoteRepository<StringNote>, StringNote>);
static_assert(notes::NoteRepositoryConcept<StringRepo, StringNote>);

using IntNote = Note<IntegerId>;
using IntRepo = notes::MarkdownNoteRepository<IntNote>;

static_assert(NoteRepositoryConcept<InMemoryNoteRepository<IntNote>, IntNote>);
static_assert(notes::NoteRepositoryConcept<IntRepo, IntNote>);

using IntReminder = Reminder<IntegerId>;
using IntReminderMemRepo = InMemoryNoteRepository<IntReminder>;
using IntReminderMdRepo = notes::MarkdownReminderRepository<IntReminder>;

static_assert(NoteRepositoryConcept<IntReminderMemRepo, IntReminder>);
static_assert(notes::NoteRepositoryConcept<IntReminderMdRepo, IntReminder>);
