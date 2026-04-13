#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Note.hpp"
#include "Reminder.hpp"
#include "NoteIdConcept.hpp"
#include "NoteRepositoryConcept.hpp"
#include "StringId.hpp"
#include "IntegerId.hpp"

namespace notes::tests {

using NoteType = Note<StringId>;
using IntNoteType = Note<IntegerId>;
using IntReminderType = Reminder<IntegerId>;

struct InMemoryRepoMock {
    void save(const NoteType&) {}
    void remove(const NoteType::IdType&) {}
    std::optional<NoteType> get(const NoteType::IdType&) const { return std::nullopt; }
    std::vector<NoteType> list() const { return {}; }
};

static_assert(NoteIdConcept<StringId>);
static_assert(NoteIdConcept<IntegerId>);
static_assert(NoteRepositoryConcept<InMemoryRepoMock, NoteType>);

} // namespace notes::tests