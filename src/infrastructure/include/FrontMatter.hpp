#pragma once

#include <filesystem>
#include <fstream>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_map>

namespace notes::infra {

struct FrontMatterDocument {
    std::unordered_map<std::string, std::string> fields;
    std::string content;
};

inline std::optional<FrontMatterDocument> read_front_matter(const std::filesystem::path& path) {
    std::ifstream in(path);
    if (!in) {
        return std::nullopt;
    }

    std::string line;
    std::getline(in, line);
    if (line != "---") {
        return std::nullopt;
    }

    FrontMatterDocument doc;
    while (std::getline(in, line)) {
        if (line == "---") {
            break;
        }

        const auto pos = line.find(": ");
        if (pos == std::string::npos) {
            continue;
        }

        doc.fields.emplace(line.substr(0, pos), line.substr(pos + 2));
    }

    std::ostringstream oss;
    oss << in.rdbuf();
    doc.content = oss.str();

    return doc;
}

inline std::string field_or_empty(const FrontMatterDocument& doc, const std::string& key) {
    auto it = doc.fields.find(key);
    return it == doc.fields.end() ? std::string{} : it->second;
}

} // namespace notes::infra
