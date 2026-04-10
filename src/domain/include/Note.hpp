#pragma once
#include <string>
#include <chrono>
#include <compare>
#include "NoteId.hpp"

namespace notese {

    // -----------------------------
    // Note entity (domain model)
    // -----------------------------
    class Note {
    public:
        using Clock = std::chrono::system_clock;
        using TimePoint = std::chrono::time_point<Clock>;

        Note(NoteId id,
             std::string title,
             std::string content,
             TimePoint created_at,
             TimePoint updated_at);

        // Factory for new notes
        static Note CreateNew(NoteId id, std::string title, std::string content);

        // Accessors
        const NoteId& id() const noexcept { return id_; }
        const std::string& title() const noexcept { return title_; }
        const std::string& content() const noexcept { return content_; }
        TimePoint created_at() const noexcept { return created_at_; }
        TimePoint updated_at() const noexcept { return updated_at_; }

        // Domain behavior: editing content
        Note with_updated_content(std::string new_content) const;

        // Domain behavior: editing title
        Note with_updated_title(std::string new_title) const;

        // Comparison (value semantics)
        auto operator<=>(const Note&) const = default;

    private:
        NoteId id_;
        std::string title_;
        std::string content_;
        TimePoint created_at_;
        TimePoint updated_at_;
    };

} // namespace notes
