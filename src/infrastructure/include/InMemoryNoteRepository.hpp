#include <unordered_map>
#include <optional>
#include <vector>

template <typename Note>
class InMemoryNoteRepository {
public:
    using Id = typename Note::IdType;

    void save(const Note& note) {
        notes_.insert_or_assign(note.id(), note);
    }

    void remove(const Id& id) {
        notes_.erase(id);
    }

    std::optional<Note> get(const Id& id) const {
        if (auto it = notes_.find(id); it != notes_.end())
            return it->second;
        return std::nullopt;
    }

    std::vector<Note> list() const {
        std::vector<Note> out;
        out.reserve(notes_.size());
        for (auto& [_, note] : notes_)
            out.push_back(note);
        return out;
    }

private:
    std::unordered_map<Id, Note> notes_;
};
