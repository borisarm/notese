#pragma once
#include <string>
#include <chrono>
#include <compare>
#include "NoteIdConcept.hpp"

namespace notes {

    template <NoteIdConcept NoteId>
    class Reminder {
    public:
        using IdType = NoteId;
        using Clock = std::chrono::system_clock;
        using TimePoint = std::chrono::time_point<Clock>;

        Reminder(NoteId id,
                 std::string title,
                 std::string content,
                 TimePoint remind_at,
                 TimePoint created_at,
                 TimePoint updated_at)
            : id_(std::move(id)),
              title_(std::move(title)),
              content_(std::move(content)),
              remind_at_(remind_at),
              created_at_(created_at),
              updated_at_(updated_at) {}

        static Reminder CreateNew(NoteId id, std::string title, std::string content, TimePoint remind_at) {
            auto now = Clock::now();
            return Reminder(std::move(id), std::move(title), std::move(content), remind_at, now, now);
        }

        // Getters
        const NoteId& id() const noexcept { return id_; }
        const std::string& title() const noexcept { return title_; }
        const std::string& content() const noexcept { return content_; }
        TimePoint remind_at() const noexcept { return remind_at_; }
        TimePoint created_at() const noexcept { return created_at_; }
        TimePoint updated_at() const noexcept { return updated_at_; }

        // Functional modifications
        Reminder with_updated_content(std::string new_content) const {
            return Reminder(id_, title_, std::move(new_content), remind_at_, created_at_, Clock::now());
        }

        Reminder with_updated_title(std::string new_title) const {
            return Reminder(id_, std::move(new_title), content_, remind_at_, created_at_, Clock::now());
        }

        Reminder with_updated_remind_at(TimePoint new_remind_at) const {
            return Reminder(id_, title_, content_, new_remind_at, created_at_, Clock::now());
        }

        auto operator<=>(const Reminder&) const = default;

    private:
        NoteId id_;
        std::string title_;
        std::string content_;
        TimePoint remind_at_;
        TimePoint created_at_;
        TimePoint updated_at_;
    };

} // namespace notes
