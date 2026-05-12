#pragma once

#include <filesystem>
#include <optional>
#include <vector>
#include "FileWriter.hpp"
#include "FrontMatter.hpp"
#include "Iso8601.hpp"
#include "NextIdAllocator.hpp"
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

        void save(const Note& note) {
            auto path = file_path(note.id());
            auto out = infra::open_for_write(path);

            out << "---\n";
            out << "id: " << note.id().to_string() << "\n";
            out << "title: " << note.title() << "\n";
            out << "created_at: " << infra::to_iso8601(note.created_at()) << "\n";
            out << "updated_at: " << infra::to_iso8601(note.updated_at()) << "\n";
            out << "---\n\n";
            out << note.content();
        }

        void remove(const Id& id) {
            std::filesystem::remove(file_path(id));
        }

        std::optional<Note> get(const Id& id) const {
            auto path = file_path(id);
            if (!std::filesystem::exists(path)) {
                return std::nullopt;
            }
            return load_from_file(path);
        }

        std::vector<Note> list() const {
            std::vector<Note> notes;
            for (auto& entry : std::filesystem::directory_iterator(dir_)) {
                if (entry.is_regular_file() && entry.path().extension() == ".md") {
                    if (auto note = load_from_file(entry.path())) {
                        notes.push_back(*note);
                    }
                }
            }
            return notes;
        }

        Id next_id() const {
            return infra::next_integer_id_in_directory<Id>(dir_);
        }

    private:
        std::filesystem::path dir_;

        std::filesystem::path file_path(const Id& id) const {
            return dir_ / (id.to_string() + ".md");
        }

        std::optional<Note> load_from_file(const std::filesystem::path& path) const {
            auto doc = infra::read_front_matter(path);
            if (!doc) {
                return std::nullopt;
            }

            return Note(
                Id{infra::field_or_empty(*doc, "id")},
                infra::field_or_empty(*doc, "title"),
                doc->content,
                infra::from_iso8601<TimePoint>(infra::field_or_empty(*doc, "created_at")),
                infra::from_iso8601<TimePoint>(infra::field_or_empty(*doc, "updated_at")));
        }
    };

} // namespace notes
