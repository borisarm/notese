#pragma once
#include <vector>
#include <optional>
#include <concepts>
#include "Note.hpp"

namespace notes {

    template <typename Repo, typename Note>
    concept NoteRepositoryConcept =
        requires(Repo repo, const Repo crepo, const Note& note, const typename Note::IdType& id) {

            // save(note) -> void
            { repo.save(note) } -> std::same_as<void>;

            // remove(id) -> void
            { repo.remove(id) } -> std::same_as<void>;

            // get(id) -> optional<Note>
            { crepo.get(id) } -> std::same_as<std::optional<Note>>;

            // list() -> vector<Note>
            { crepo.list() } -> std::same_as<std::vector<Note>>;
        };
}
