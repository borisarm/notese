#pragma once
#include <filesystem>
#include <fstream>
#include <optional>
#include <vector>
#include <sstream>
#include "Note.hpp"
#include "NoteRepositoryConcept.hpp"

namespace notes {

    template <typename Note>
    class MarkdownNoteRepository {
    public:
        using Id = typename Note::IdType;
        using Clock = typename Note::Clock;
        using TimePoint = typename Note::TimePoint;

        explicit MarkdownNoteRepository(std::filesystem::path directory)
            : dir_(std::move(directory)) 
        {
            std::filesystem::create_directories(dir_);
        }

        // -----------------------------------------
        // save(note)
        // -----------------------------------------
        void save(const Note& note) {
            auto path = file_path(note.id());
            std::ofstream out(path);

            out << "---\n";
            out << "id: " << note.id().to_string() << "\n";
            out << "title: " << note.title() << "\n";
            out << "created_at: " << to_iso(note.created_at()) << "\n";
            out << "updated_at: " << to_iso(note.updated_at()) << "\n";
            out << "---\n\n";
            out << note.content();
        }

        // -----------------------------------------
        // remove(id)
        // -----------------------------------------
        void remove(const Id& id) {
            auto path = file_path(id);
            std::filesystem::remove(path);
        }

        // -----------------------------------------
        // get(id)
        // -----------------------------------------
        std::optional<Note> get(const Id& id) const {
            auto path = file_path(id);
            if (!std::filesystem::exists(path))
                return std::nullopt;

            return load_from_file(path);
        }

        // -----------------------------------------
        // list()
        // -----------------------------------------
        std::vector<Note> list() const {
            std::vector<Note> notes;

            for (auto& entry : std::filesystem::directory_iterator(dir_)) {
                if (entry.is_regular_file() && entry.path().extension() == ".md") {
                    if (auto note = load_from_file(entry.path()))
                        notes.push_back(*note);
                }
            }

            return notes;
        }

        // -----------------------------------------
        // next_id()
        // -----------------------------------------
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

        // -----------------------------------------
        // Parse Markdown file
        // -----------------------------------------
        std::optional<Note> load_from_file(const std::filesystem::path& path) const {
            std::ifstream in(path);
            if (!in)
                return std::nullopt;

            std::string line;
            std::string id, title, created, updated;
            std::string content;

            // Expect front-matter
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
                else if (key == "created_at") created = value;
                else if (key == "updated_at") updated = value;
            }

            // Read content
            std::ostringstream oss;
            oss << in.rdbuf();
            content = oss.str();

            return Note(
                Id{id},
                title,
                content,
                from_iso(created),
                from_iso(updated)
            );
        }

        // -----------------------------------------
        // ISO8601 helpers
        // -----------------------------------------
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
