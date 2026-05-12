#pragma once

#include <ftxui/dom/elements.hpp>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

namespace notes::tui {

inline bool is_ordered_list_item(const std::string& line) {
    size_t i = 0;
    while (i < line.size() && std::isdigit(static_cast<unsigned char>(line[i]))) {
        ++i;
    }
    return i > 0 && (i + 1) < line.size() && line[i] == '.' && line[i + 1] == ' ';
}

inline ftxui::Element render_markdown_preview(const std::string& markdown) {
    using namespace ftxui;

    if (markdown.empty()) {
        return text("(empty)") | dim;
    }

    std::istringstream iss(markdown);
    std::string line;
    std::vector<Element> rows;
    bool in_code_fence = false;

    while (std::getline(iss, line)) {
        if (line.rfind("```", 0) == 0) {
            in_code_fence = !in_code_fence;
            rows.push_back(text(in_code_fence ? "[code]" : "[/code]") | dim);
            continue;
        }

        if (in_code_fence) {
            rows.push_back(text("  " + line) | color(Color::CyanLight));
            continue;
        }

        if (line.empty()) {
            rows.push_back(text(""));
            continue;
        }

        if (line.rfind("###### ", 0) == 0 || line.rfind("##### ", 0) == 0 ||
            line.rfind("#### ", 0) == 0 || line.rfind("### ", 0) == 0 ||
            line.rfind("## ", 0) == 0 || line.rfind("# ", 0) == 0) {
            size_t level = 0;
            while (level < line.size() && line[level] == '#') {
                ++level;
            }
            auto heading_text = line.substr(std::min(line.size(), level + 1));
            if (level <= 2) {
                rows.push_back(text(heading_text) | bold | underlined);
            } else {
                rows.push_back(text(heading_text) | bold);
            }
            continue;
        }

        if (line.rfind("> ", 0) == 0) {
            rows.push_back(hbox({text("| ") | dim, paragraph(line.substr(2)) | dim}));
            continue;
        }

        if (line == "---" || line == "***") {
            rows.push_back(separator());
            continue;
        }

        if (line.rfind("- ", 0) == 0 || line.rfind("* ", 0) == 0 || line.rfind("+ ", 0) == 0) {
            rows.push_back(hbox({text("• ") | color(Color::YellowLight), paragraph(line.substr(2)) | flex}));
            continue;
        }

        if (is_ordered_list_item(line)) {
            auto dot = line.find('.');
            auto index = line.substr(0, dot + 1);
            auto content = line.substr(dot + 2);
            rows.push_back(hbox({text(index + " ") | color(Color::YellowLight), paragraph(content) | flex}));
            continue;
        }

        rows.push_back(paragraph(line));
    }

    return vbox(std::move(rows));
}

} // namespace notes::tui
