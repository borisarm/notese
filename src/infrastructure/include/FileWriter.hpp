#pragma once

#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>

namespace notes::infra {

class FileWriteError : public std::runtime_error {
public:
    explicit FileWriteError(const std::filesystem::path& path)
        : std::runtime_error("Failed to write file: " + path.string()) {}
};

inline std::ofstream open_for_write(const std::filesystem::path& path) {
    std::ofstream out(path);
    if (!out) {
        throw FileWriteError(path);
    }
    return out;
}

} // namespace notes::infra
