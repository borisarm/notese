#pragma once
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <algorithm>
#include <string>
#include <vector>
#include "Note.hpp"
#include "Reminder.hpp"
#include "IntegerId.hpp"
#include "NoteRepositoryConcept.hpp"

namespace notes::tui {

using NoteType = Note<IntegerId>;
using ReminderType = Reminder<IntegerId>;

template <NoteRepositoryConcept<NoteType> NoteRepo, NoteRepositoryConcept<ReminderType> ReminderRepo>
int run(NoteRepo& note_repo, ReminderRepo& reminder_repo) {
    using namespace ftxui;

    auto notes = note_repo.list();
    std::sort(notes.begin(), notes.end(), [](const NoteType& a, const NoteType& b) {
        return a.title() < b.title();
    });

    auto reminders = reminder_repo.list();
    std::sort(reminders.begin(), reminders.end(), [](const ReminderType& a, const ReminderType& b) {
        return a.remind_at() < b.remind_at();
    });

    int selected = 0;
    int tab = 0;

    std::vector<std::string> note_entries;
    for (const auto& n : notes) {
        note_entries.push_back(n.id().to_string() + "  " + n.title());
    }

    std::vector<std::string> reminder_entries;
    for (const auto& r : reminders) {
        reminder_entries.push_back(r.id().to_string() + "  " + std::format("{:%F}", r.remind_at()) + "  " + r.title());
    }

    auto note_menu = Menu(&note_entries, &selected);
    auto reminder_menu = Menu(&reminder_entries, &selected);

    auto content_view = Renderer(note_menu, [&] {
        Element detail;

        if (tab == 0) {
            if (notes.empty()) {
                detail = vbox({
                    text("No notes found.") | center | dim,
                    filler(),
                    text("Use 'notese add <title>' to create a note.") | dim | center,
                });
            } else {
                const auto& note = notes[selected];
                detail = vbox({
                    text(note.title()) | bold,
                    text("id: " + note.id().to_string()) | dim,
                    text("updated: " + std::format("{:%F %T}", note.updated_at())) | dim,
                    separator(),
                    paragraph(note.content()) | flex,
                }) | flex;
            }

            return vbox({
                text("notese") | bold | center,
                separator(),
                hbox({
                    vbox({
                        text("Notes") | bold | inverted,
                        text("Reminders") | bold | dim,
                        separator(),
                        note_menu->Render() | vscroll_indicator | frame | size(WIDTH, EQUAL, 30),
                    }),
                    separator(),
                    detail | flex,
                }) | flex,
                separator(),
                text("Tab switch  ↑/↓ navigate  q quit") | dim | center,
            });
        } else {
            if (reminders.empty()) {
                detail = vbox({
                    text("No reminders found.") | center | dim,
                    filler(),
                    text("Use 'notese remind <title> <date>' to create a reminder.") | dim | center,
                });
            } else {
                const auto& r = reminders[selected];
                detail = vbox({
                    text(r.title()) | bold,
                    text("id: " + r.id().to_string()) | dim,
                    text("remind: " + std::format("{:%F}", r.remind_at())) | dim,
                    text("updated: " + std::format("{:%F %T}", r.updated_at())) | dim,
                    separator(),
                    paragraph(r.content()) | flex,
                }) | flex;
            }

            return vbox({
                text("notese") | bold | center,
                separator(),
                hbox({
                    vbox({
                        text("Notes") | bold | dim,
                        text("Reminders") | bold | inverted,
                        separator(),
                        reminder_menu->Render() | vscroll_indicator | frame | size(WIDTH, EQUAL, 40),
                    }),
                    separator(),
                    detail | flex,
                }) | flex,
                separator(),
                text("Tab switch  ↑/↓ navigate  q quit") | dim | center,
            });
        }
    });

    auto screen = ScreenInteractive::Fullscreen();

    auto with_quit = CatchEvent(content_view, [&](Event event) {
        if (event == Event::Character('q') || event == Event::Escape) {
            screen.Exit();
            return true;
        }
        if (event == Event::Tab) {
            tab = (tab + 1) % 2;
            selected = 0;
            return true;
        }
        return false;
    });

    screen.Loop(with_quit);
    return 0;
}

} // namespace notes::tui
