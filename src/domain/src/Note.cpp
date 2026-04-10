#include "Note.hpp"

namespace notese {

    // -----------------------------
    // Strong type for NoteId
    // -----------------------------
   

    // -----------------------------
    // Note entity (domain model)
    // -----------------------------
    Note::Note(NoteId id,
             std::string title,
             std::string content,
             TimePoint created_at,
             TimePoint updated_at)
            : id_(std::move(id)),
              title_(std::move(title)),
              content_(std::move(content)),
              created_at_(created_at),
              updated_at_(updated_at) {}

        // Factory for new notes
        Note Note::CreateNew(NoteId id, std::string title, std::string content) {
            auto now = Clock::now();
            return Note(std::move(id), std::move(title), std::move(content), now, now);
        }

        // Accessors
        
        // Domain behavior: editing content
        Note Note::with_updated_content(std::string new_content) const {
            return Note(id_, title_, std::move(new_content), created_at_, Clock::now());
        }

        // Domain behavior: editing title
        Note Note::with_updated_title(std::string new_title) const {
            return Note(id_, std::move(new_title), content_, created_at_, Clock::now());
        }

} // namespace notes
