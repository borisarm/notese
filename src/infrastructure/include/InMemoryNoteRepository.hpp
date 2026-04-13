#include <unordered_map>
#include <optional>
#include <vector>
#include <algorithm>
#include <type_traits>

template <typename Note>
class InMemoryNoteRepository {
public:
    using Id = typename Note::IdType;

    void save(const Note& note) {
        notes_.insert_or_assign(note.id(), note);
        if constexpr (std::is_integral_v<decltype(Id::value)>) {
            if (note.id().value >= next_id_)
                next_id_ = note.id().value + 1;
        }
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

    Id next_id() {
        return Id{next_id_++};
    }

private:
    std::unordered_map<Id, Note> notes_;
    int next_id_ = 1;
};
