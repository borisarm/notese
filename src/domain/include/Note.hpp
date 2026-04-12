#pragma once
#include <string>
#include <chrono>
#include <compare>
#include "NoteIdConcept.hpp"

namespace notes {

    template <NoteIdConcept NoteId>
    class Note {
    public:
        using Clock = std::chrono::system_clock;
        using TimePoint = std::chrono::time_point<Clock>;

        Note(NoteId id,
             std::string title,
             std::string content,
             TimePoint created_at,
             TimePoint updated_at)
            : id_(std::move(id)),
              title_(std::move(title)),
              content_(std::move(content)),
              created_at_(created_at),
              updated_at_(updated_at) {}

        static Note CreateNew(NoteId id, std::string title, std::string content) {
            auto now = Clock::now();
            return Note(std::move(id), std::move(title), std::move(content), now, now);
        }

        // Getters
        const NoteId& id() const noexcept { return id_; }
        const std::string& title() const noexcept { return title_; }
        const std::string& content() const noexcept { return content_; }
        TimePoint created_at() const noexcept { return created_at_; }
        TimePoint updated_at() const noexcept { return updated_at_; }

        // Modificaciones funcionales
        Note with_updated_content(std::string new_content) const {
            return Note(id_, title_, std::move(new_content), created_at_, Clock::now());
        }

        Note with_updated_title(std::string new_title) const {
            return Note(id_, std::move(new_title), content_, created_at_, Clock::now());
        }

        auto operator<=>(const Note&) const = default;

    private:
        NoteId id_;
        std::string title_;
        std::string content_;
        TimePoint created_at_;
        TimePoint updated_at_;
    };

} // namespace notes
