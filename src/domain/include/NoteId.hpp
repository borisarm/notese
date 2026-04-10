#pragma once

#include <string>

namespace notese {

class NoteId {
    public:
        explicit NoteId(std::string value) : value_(std::move(value)) {}

        const std::string& value() const noexcept { return value_; }

        auto operator<=>(const NoteId&) const = default;

    private:
        std::string value_;
    };
}