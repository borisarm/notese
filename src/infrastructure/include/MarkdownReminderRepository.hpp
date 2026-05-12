#pragma once

#include <filesystem>
#include <fstream>
#include <optional>
#include <vector>
#include "FrontMatter.hpp"
#include "Iso8601.hpp"
#include "NextIdAllocator.hpp"
#include "NoteRepositoryConcept.hpp"
#include "Reminder.hpp"

namespace notes {

    template <typename Reminder>
    class MarkdownReminderRepository {
    public:
        using Id = typename Reminder::IdType;
        using Clock = typename Reminder::Clock;
        using TimePoint = typename Reminder::TimePoint;

        explicit MarkdownReminderRepository(std::filesystem::path directory)
            : dir_(std::move(directory))
        {
            std::filesystem::create_directories(dir_);
        }

        void save(const Reminder& reminder) {
            auto path = file_path(reminder.id());
            std::ofstream out(path);

            out << "---\n";
            out << "id: " << reminder.id().to_string() << "\n";
            out << "title: " << reminder.title() << "\n";
            out << "remind_at: " << infra::to_iso8601(reminder.remind_at()) << "\n";
            out << "created_at: " << infra::to_iso8601(reminder.created_at()) << "\n";
            out << "updated_at: " << infra::to_iso8601(reminder.updated_at()) << "\n";
            out << "---\n\n";
            out << reminder.content();
        }

        void remove(const Id& id) {
            std::filesystem::remove(file_path(id));
        }

        std::optional<Reminder> get(const Id& id) const {
            auto path = file_path(id);
            if (!std::filesystem::exists(path)) {
                return std::nullopt;
            }
            return load_from_file(path);
        }

        std::vector<Reminder> list() const {
            std::vector<Reminder> reminders;
            for (auto& entry : std::filesystem::directory_iterator(dir_)) {
                if (entry.is_regular_file() && entry.path().extension() == ".md") {
                    if (auto r = load_from_file(entry.path())) {
                        reminders.push_back(*r);
                    }
                }
            }
            return reminders;
        }

        Id next_id() const {
            return infra::next_integer_id_in_directory<Id>(dir_);
        }

    private:
        std::filesystem::path dir_;

        std::filesystem::path file_path(const Id& id) const {
            return dir_ / (id.to_string() + ".md");
        }

        std::optional<Reminder> load_from_file(const std::filesystem::path& path) const {
            auto doc = infra::read_front_matter(path);
            if (!doc) {
                return std::nullopt;
            }

            return Reminder(
                Id{infra::field_or_empty(*doc, "id")},
                infra::field_or_empty(*doc, "title"),
                doc->content,
                infra::from_iso8601<TimePoint>(infra::field_or_empty(*doc, "remind_at")),
                infra::from_iso8601<TimePoint>(infra::field_or_empty(*doc, "created_at")),
                infra::from_iso8601<TimePoint>(infra::field_or_empty(*doc, "updated_at")));
        }
    };

} // namespace notes
