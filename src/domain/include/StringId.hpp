#include <string>
#include <compare>

namespace notes {

    struct StringId {

        StringId(std::string value) : value(std::move(value)) {}

        std::string value;

        std::string to_string() const { return value; }

        auto operator<=>(const StringId&) const = default;
    };

}