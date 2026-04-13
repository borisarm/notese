#pragma once
#include <filesystem>
#include <fstream>
#include <optional>
#include <vector>
#include <sstream>
#include "Reminder.hpp"
#include "NoteRepositoryConcept.hpp"

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
            out << "remind_at: " << to_iso(reminder.remind_at()) << "\n";
            out << "created_at: " << to_iso(reminder.created_at()) << "\n";
            out << "updated_at: " << to_iso(reminder.updated_at()) << "\n";
            out << "---\n\n";
            out << reminder.content();
        }

        void remove(const Id& id) {
            auto path = file_path(id);
            std::filesystem::remove(path);
        }

        std::optional<Reminder> get(const Id& id) const {
            auto path = file_path(id);
            if (!std::filesystem::exists(path))
                return std::nullopt;

            return load_from_file(path);
        }

        std::vector<Reminder> list() const {
            std::vector<Reminder> reminders;

            for (auto& entry : std::filesystem::directory_iterator(dir_)) {
                if (entry.is_regular_file() && entry.path().extension() == ".md") {
                    if (auto r = load_from_file(entry.path()))
                        reminders.push_back(*r);
                }
            }

            return reminders;
        }

        Id next_id() const {
            int max_id = 0;
            for (auto& entry : std::filesystem::directory_iterator(dir_)) {
                if (entry.is_regular_file() && entry.path().extension() == ".md") {
                    try {
                        int id = std::stoi(entry.path().stem().string());
                        if (id > max_id) max_id = id;
                    } catch (...) {}
                }
            }
            return Id{max_id + 1};
        }

    private:
        std::filesystem::path dir_;

        std::filesystem::path file_path(const Id& id) const {
            return dir_ / (id.to_string() + ".md");
        }

        std::optional<Reminder> load_from_file(const std::filesystem::path& path) const {
            std::ifstream in(path);
            if (!in)
                return std::nullopt;

            std::string line;
            std::string id, title, remind, created, updated;
            std::string content;

            std::getline(in, line);
            if (line != "---")
                return std::nullopt;

            while (std::getline(in, line)) {
                if (line == "---")
                    break;

                auto pos = line.find(": ");
                if (pos == std::string::npos)
                    continue;

                auto key = line.substr(0, pos);
                auto value = line.substr(pos + 2);

                if (key == "id") id = value;
                else if (key == "title") title = value;
                else if (key == "remind_at") remind = value;
                else if (key == "created_at") created = value;
                else if (key == "updated_at") updated = value;
            }

            std::ostringstream oss;
            oss << in.rdbuf();
            content = oss.str();

            return Reminder(
                Id{id},
                title,
                content,
                from_iso(remind),
                from_iso(created),
                from_iso(updated)
            );
        }

        static std::string to_iso(TimePoint tp) {
            return std::format("{:%FT%TZ}", tp);
        }

        static TimePoint from_iso(const std::string& s) {
            TimePoint tp{};
            std::istringstream iss(s);
            iss >> std::chrono::parse("%FT%TZ", tp);
            return tp;
        }

    };

} // namespace notes
