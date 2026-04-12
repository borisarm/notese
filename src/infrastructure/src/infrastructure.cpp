#include "InMemoryNoteRepository.hpp"
#include "NoteRepositoryConcept.hpp"
#include "Note.hpp"
#include "StringId.hpp"
#include "MarkdownNoteRepository.hpp"

using namespace notes;

using MyNote = Note<StringId>;
using Repo = notes::MarkdownNoteRepository<MyNote>;

static_assert(NoteRepositoryConcept<InMemoryNoteRepository<MyNote>, MyNote>);
static_assert(notes::NoteRepositoryConcept<Repo, MyNote>);
