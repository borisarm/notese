#include <string>
#include <compare>
#include <functional>

namespace notes {

    struct StringId {

        StringId(std::string value) : value(std::move(value)) {}

        std::string value;

        std::string to_string() const { return value; }

        auto operator<=>(const StringId&) const = default;
    };

}

template<>
struct std::hash<notes::StringId> {
    std::size_t operator()(const notes::StringId& id) const noexcept {
        return std::hash<std::string>{}(id.value);
    }
};