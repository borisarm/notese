#pragma once

#include <ftxui/dom/elements.hpp>
#include <string>

namespace notes::tui::visual {

inline ftxui::Element app_title() {
    using namespace ftxui;
    return text("notese") | bold | center;
}

inline ftxui::Element help_footer() {
    using namespace ftxui;
    return text("Tab switch  ↑/↓ navigate  a add  e edit  d delete  q quit") | dim | center;
}

inline ftxui::Element status_bar(const std::string& message) {
    using namespace ftxui;
    if (message.empty()) {
        return text("");
    }
    return text(message) | color(Color::Red) | center;
}

inline ftxui::Element empty_state(const std::string& message, const std::string& hint) {
    using namespace ftxui;
    return vbox({
        text(message) | center | dim,
        filler(),
        text(hint) | dim | center,
    });
}

} // namespace notes::tui::visual
