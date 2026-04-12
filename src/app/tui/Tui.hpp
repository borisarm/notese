#pragma once
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <algorithm>
#include <string>
#include <vector>
#include "Note.hpp"
#include "StringId.hpp"
#include "MarkdownNoteRepository.hpp"

namespace notes::tui {

using NoteType = Note<StringId>;
using Repo = MarkdownNoteRepository<NoteType>;

inline int run(Repo& repo) {
    using namespace ftxui;

    auto notes = repo.list();
    std::sort(notes.begin(), notes.end(), [](const NoteType& a, const NoteType& b) {
        return a.title() < b.title();
    });

    int selected = 0;
    std::vector<std::string> entries;
    for (const auto& n : notes) {
        entries.push_back(n.id().to_string() + "  " + n.title());
    }

    auto menu = Menu(&entries, &selected);

    auto content_view = Renderer(menu, [&] {
        if (notes.empty()) {
            return vbox({
                text("notese") | bold | center,
                separator(),
                text("No notes found.") | center | dim,
                filler(),
                text("Use 'notese add <id> <title>' to create a note.") | dim | center,
            });
        }

        const auto& note = notes[selected];

        return vbox({
            text("notese") | bold | center,
            separator(),
            hbox({
                vbox({
                    text("Notes") | bold,
                    separator(),
                    menu->Render() | vscroll_indicator | frame | size(WIDTH, EQUAL, 30),
                }),
                separator(),
                vbox({
                    text(note.title()) | bold,
                    text("id: " + note.id().to_string()) | dim,
                    text("updated: " + std::format("{:%F %T}", note.updated_at())) | dim,
                    separator(),
                    paragraph(note.content()) | flex,
                }) | flex,
            }) | flex,
            separator(),
            text("↑/↓ navigate  q quit") | dim | center,
        });
    });

    auto screen = ScreenInteractive::Fullscreen();

    auto with_quit = CatchEvent(content_view, [&](Event event) {
        if (event == Event::Character('q') || event == Event::Escape) {
            screen.Exit();
            return true;
        }
        return false;
    });

    screen.Loop(with_quit);
    return 0;
}

} // namespace notes::tui
