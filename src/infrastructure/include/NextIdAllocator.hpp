#pragma once

#include <exception>
#include <filesystem>
#include <string>

namespace notes::infra {

template <typename Id>
inline Id next_integer_id_in_directory(const std::filesystem::path& dir) {
    int max_id = 0;
    for (auto& entry : std::filesystem::directory_iterator(dir)) {
        if (!entry.is_regular_file() || entry.path().extension() != ".md") {
            continue;
        }

        const auto stem = entry.path().stem().string();
        try {
            std::size_t consumed = 0;
            const int id = std::stoi(stem, &consumed);
            if (consumed != stem.size()) {
                continue;
            }
            if (id > max_id) {
                max_id = id;
            }
        } catch (const std::invalid_argument&) {
            // Filename stem is not a valid integer id; ignore.
        } catch (const std::out_of_range&) {
            // Filename stem is too large to fit in int; ignore.
        }
    }
    return Id{max_id + 1};
}

} // namespace notes::infra
