#pragma once
#include <concepts>
#include <string>
#include <compare>

namespace notes {

    template <typename T>
    concept NoteIdConcept =
        std::copyable<T> &&
        requires(const T& a, const T& b) {
            { a == b } -> std::convertible_to<bool>;
            { a <=> b } -> std::same_as<std::strong_ordering>;
            { a.to_string() } -> std::convertible_to<std::string>;
        };

} // namespace notes
