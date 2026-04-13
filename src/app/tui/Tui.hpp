#pragma once
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <algorithm>
#include <sstream>
#include <string>
#include <vector>
#include "Note.hpp"
#include "Reminder.hpp"
#include "IntegerId.hpp"
#include "NoteRepositoryConcept.hpp"

namespace notes::tui {

using NoteType = Note<IntegerId>;
using ReminderType = Reminder<IntegerId>;

enum class Mode { Browse, AddNote, EditNote, ConfirmDeleteNote,
                  AddReminder, EditReminder, ConfirmDeleteReminder };

inline auto parse_date(const std::string& s) {
    using Clock = std::chrono::system_clock;
    using TimePoint = std::chrono::time_point<Clock>;
    TimePoint tp{};
    std::istringstream iss(s);
    iss >> std::chrono::parse("%F", tp);
    return tp;
}

template <NoteRepositoryConcept<NoteType> NoteRepo, NoteRepositoryConcept<ReminderType> ReminderRepo>
int run(NoteRepo& note_repo, ReminderRepo& reminder_repo) {
    using namespace ftxui;

    // --- State ---
    int note_selected = 0;
    int reminder_selected = 0;
    int tab = 0; // 0 = Notes, 1 = Reminders
    Mode mode = Mode::Browse;
    std::string form_title;
    std::string form_content;
    std::string form_date;

    // --- Data ---
    std::vector<NoteType> notes;
    std::vector<std::string> note_entries;
    std::vector<ReminderType> reminders;
    std::vector<std::string> reminder_entries;

    auto refresh_notes = [&] {
        notes = note_repo.list();
        std::sort(notes.begin(), notes.end(), [](const NoteType& a, const NoteType& b) {
            return a.title() < b.title();
        });
        note_entries.clear();
        for (const auto& n : notes)
            note_entries.push_back(n.id().to_string() + "  " + n.title());
        if (note_selected >= (int)notes.size())
            note_selected = std::max(0, (int)notes.size() - 1);
    };

    auto refresh_reminders = [&] {
        reminders = reminder_repo.list();
        std::sort(reminders.begin(), reminders.end(), [](const ReminderType& a, const ReminderType& b) {
            return a.remind_at() < b.remind_at();
        });
        reminder_entries.clear();
        for (const auto& r : reminders)
            reminder_entries.push_back(r.id().to_string() + "  " + std::format("{:%F}", r.remind_at()) + "  " + r.title());
        if (reminder_selected >= (int)reminders.size())
            reminder_selected = std::max(0, (int)reminders.size() - 1);
    };

    refresh_notes();
    refresh_reminders();

    // --- Components ---
    auto input_title = Input(&form_title, "Title");
    auto input_content = Input(&form_content, "Content");
    auto input_date = Input(&form_date, "YYYY-MM-DD");

    auto note_menu = Menu(&note_entries, &note_selected);
    auto reminder_menu = Menu(&reminder_entries, &reminder_selected);

    // Separate containers for each mode to avoid cross-contamination
    auto form_note_container = Container::Vertical({input_title, input_content});
    auto form_reminder_container = Container::Vertical({input_title, input_content, input_date});

    auto screen = ScreenInteractive::Fullscreen();

    auto main_component = Renderer(Container::Stacked({
        form_note_container,
        form_reminder_container,
        note_menu,
        reminder_menu,
    }), [&]() -> Element {
        // --- Form views ---
        if (mode == Mode::AddNote || mode == Mode::EditNote) {
            auto title = (mode == Mode::AddNote) ? "Add Note" : "Edit Note";
            return vbox({
                text(title) | bold | center,
                separator(),
                hbox(text("Title:   "), input_title->Render() | flex),
                separator(),
                hbox(text("Content: "), input_content->Render() | flex),
                filler(),
                separator(),
                text("Enter confirm  Escape cancel") | dim | center,
            }) | border;
        }

        if (mode == Mode::AddReminder || mode == Mode::EditReminder) {
            auto title = (mode == Mode::AddReminder) ? "Add Reminder" : "Edit Reminder";
            return vbox({
                text(title) | bold | center,
                separator(),
                hbox(text("Title:   "), input_title->Render() | flex),
                hbox(text("Date:    "), input_date->Render() | flex),
                separator(),
                hbox(text("Content: "), input_content->Render() | flex),
                filler(),
                separator(),
                text("Enter confirm  Escape cancel") | dim | center,
            }) | border;
        }

        if (mode == Mode::ConfirmDeleteNote) {
            if (notes.empty()) { mode = Mode::Browse; return text(""); }
            auto& note = notes[note_selected];
            return vbox({
                text("Delete Note") | bold | center,
                separator(),
                text("Are you sure you want to delete:") | center,
                text("  " + note.title()) | bold | center,
                filler(),
                separator(),
                text("y yes  n/Escape cancel") | dim | center,
            }) | border;
        }

        if (mode == Mode::ConfirmDeleteReminder) {
            if (reminders.empty()) { mode = Mode::Browse; return text(""); }
            auto& r = reminders[reminder_selected];
            return vbox({
                text("Delete Reminder") | bold | center,
                separator(),
                text("Are you sure you want to delete:") | center,
                text("  " + r.title()) | bold | center,
                filler(),
                separator(),
                text("y yes  n/Escape cancel") | dim | center,
            }) | border;
        }

        // --- Browse view ---
        Element detail;

        if (tab == 0) {
            if (notes.empty()) {
                detail = vbox({
                    text("No notes found.") | center | dim,
                    filler(),
                    text("Press 'a' to add a note.") | dim | center,
                });
            } else {
                int idx = std::clamp(note_selected, 0, (int)notes.size() - 1);
                const auto& note = notes[idx];
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
                text("Tab switch  ↑/↓ navigate  a add  e edit  d delete  q quit") | dim | center,
            });
        } else {
            if (reminders.empty()) {
                detail = vbox({
                    text("No reminders found.") | center | dim,
                    filler(),
                    text("Press 'a' to add a reminder.") | dim | center,
                });
            } else {
                int idx = std::clamp(reminder_selected, 0, (int)reminders.size() - 1);
                const auto& r = reminders[idx];
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
                text("Tab switch  ↑/↓ navigate  a add  e edit  d delete  q quit") | dim | center,
            });
        }
    });

    auto with_events = CatchEvent(main_component, [&](Event event) -> bool {
        // --- Form mode: AddNote / EditNote ---
        if (mode == Mode::AddNote || mode == Mode::EditNote) {
            if (event == Event::Escape) {
                mode = Mode::Browse;
                return true;
            }
            if (event == Event::Return) {
                if (!form_title.empty()) {
                    if (mode == Mode::AddNote) {
                        auto id = note_repo.next_id();
                        auto note = NoteType::CreateNew(id, form_title, form_content);
                        note_repo.save(note);
                    } else if (!notes.empty()) {
                        int idx = std::clamp(note_selected, 0, (int)notes.size() - 1);
                        auto updated = notes[idx]
                            .with_updated_title(form_title)
                            .with_updated_content(form_content);
                        note_repo.save(updated);
                    }
                    refresh_notes();
                    mode = Mode::Browse;
                }
                return true;
            }
            return false;
        }

        // --- Form mode: AddReminder / EditReminder ---
        if (mode == Mode::AddReminder || mode == Mode::EditReminder) {
            if (event == Event::Escape) {
                mode = Mode::Browse;
                return true;
            }
            if (event == Event::Return) {
                if (!form_title.empty() && !form_date.empty()) {
                    auto remind_at = parse_date(form_date);
                    if (mode == Mode::AddReminder) {
                        auto id = reminder_repo.next_id();
                        auto r = ReminderType::CreateNew(id, form_title, form_content, remind_at);
                        reminder_repo.save(r);
                    } else if (!reminders.empty()) {
                        int idx = std::clamp(reminder_selected, 0, (int)reminders.size() - 1);
                        auto updated = reminders[idx]
                            .with_updated_title(form_title)
                            .with_updated_remind_at(remind_at)
                            .with_updated_content(form_content);
                        reminder_repo.save(updated);
                    }
                    refresh_reminders();
                    mode = Mode::Browse;
                }
                return true;
            }
            return false;
        }

        // --- Confirm delete ---
        if (mode == Mode::ConfirmDeleteNote) {
            if (event == Event::Character('y')) {
                if (!notes.empty()) {
                    int idx = std::clamp(note_selected, 0, (int)notes.size() - 1);
                    note_repo.remove(notes[idx].id());
                }
                refresh_notes();
                mode = Mode::Browse;
                return true;
            }
            if (event == Event::Character('n') || event == Event::Escape) {
                mode = Mode::Browse;
                return true;
            }
            return true;
        }

        if (mode == Mode::ConfirmDeleteReminder) {
            if (event == Event::Character('y')) {
                if (!reminders.empty()) {
                    int idx = std::clamp(reminder_selected, 0, (int)reminders.size() - 1);
                    reminder_repo.remove(reminders[idx].id());
                }
                refresh_reminders();
                mode = Mode::Browse;
                return true;
            }
            if (event == Event::Character('n') || event == Event::Escape) {
                mode = Mode::Browse;
                return true;
            }
            return true;
        }

        // --- Browse mode ---
        if (event == Event::Character('q') || event == Event::Escape) {
            screen.Exit();
            return true;
        }

        if (event == Event::Tab) {
            tab = (tab + 1) % 2;
            return true;
        }

        if (event == Event::Character('a')) {
            form_title.clear();
            form_content.clear();
            form_date.clear();
            if (tab == 0) {
                mode = Mode::AddNote;
            } else {
                mode = Mode::AddReminder;
            }
            return true;
        }

        if (event == Event::Character('e')) {
            if (tab == 0 && !notes.empty()) {
                int idx = std::clamp(note_selected, 0, (int)notes.size() - 1);
                form_title = notes[idx].title();
                form_content = notes[idx].content();
                mode = Mode::EditNote;
            } else if (tab == 1 && !reminders.empty()) {
                int idx = std::clamp(reminder_selected, 0, (int)reminders.size() - 1);
                form_title = reminders[idx].title();
                form_content = reminders[idx].content();
                form_date = std::format("{:%F}", reminders[idx].remind_at());
                mode = Mode::EditReminder;
            }
            return true;
        }

        if (event == Event::Character('d')) {
            if (tab == 0 && !notes.empty()) {
                mode = Mode::ConfirmDeleteNote;
            } else if (tab == 1 && !reminders.empty()) {
                mode = Mode::ConfirmDeleteReminder;
            }
            return true;
        }

        // Let menu handle navigation in browse mode
        if (tab == 0) {
            return note_menu->OnEvent(event);
        } else {
            return reminder_menu->OnEvent(event);
        }
    });

    screen.Loop(with_events);
    return 0;
}

} // namespace notes::tui
