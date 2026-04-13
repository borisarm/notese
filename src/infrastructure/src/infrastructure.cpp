#include "InMemoryNoteRepository.hpp"
#include "NoteRepositoryConcept.hpp"
#include "Note.hpp"
#include "StringId.hpp"
#include "IntegerId.hpp"
#include "MarkdownNoteRepository.hpp"

using namespace notes;

using StringNote = Note<StringId>;
using StringRepo = notes::MarkdownNoteRepository<StringNote>;

static_assert(NoteRepositoryConcept<InMemoryNoteRepository<StringNote>, StringNote>);
static_assert(notes::NoteRepositoryConcept<StringRepo, StringNote>);

using IntNote = Note<IntegerId>;
using IntRepo = notes::MarkdownNoteRepository<IntNote>;

static_assert(NoteRepositoryConcept<InMemoryNoteRepository<IntNote>, IntNote>);
static_assert(notes::NoteRepositoryConcept<IntRepo, IntNote>);
