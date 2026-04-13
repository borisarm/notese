#pragma once
#include <string>
#include <compare>
#include <functional>

namespace notes {

    struct IntegerId {

        IntegerId(int value) : value(value) {}

        explicit IntegerId(const std::string& s) : value(std::stoi(s)) {}

        int value;

        std::string to_string() const { return std::to_string(value); }

        auto operator<=>(const IntegerId&) const = default;
    };

}

template<>
struct std::hash<notes::IntegerId> {
    std::size_t operator()(const notes::IntegerId& id) const noexcept {
        return std::hash<int>{}(id.value);
    }
};
