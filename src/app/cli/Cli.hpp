    #pragma once
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include "Note.hpp"
#include "Reminder.hpp"
#include "IntegerId.hpp"
#include "NoteRepositoryConcept.hpp"

namespace notes::cli {

using NoteType = Note<IntegerId>;
using ReminderType = Reminder<IntegerId>;

#ifdef _WIN32
constexpr const char* eof_hint = "Enter content (Ctrl+Z then Enter to finish):";
#else
constexpr const char* eof_hint = "Enter content (Ctrl+D to finish):";
#endif

inline auto parse_date(const std::string& s) {
    using Clock = std::chrono::system_clock;
    using TimePoint = std::chrono::time_point<Clock>;

    TimePoint tp{};
    std::istringstream iss(s);
    iss >> std::chrono::parse("%F", tp);
    if (iss.fail()) {
        iss.clear();
        iss.str(s);
        iss >> std::chrono::parse("%FT%TZ", tp);
    }
    return tp;
}

inline void print_usage(const char* program) {
    std::cerr << "Usage:\n"
              << "  " << program << "                                Launch TUI\n"
              << "\n  Notes:\n"
              << "  " << program << " list                            List all notes\n"
              << "  " << program << " show <id>                       Show a note\n"
              << "  " << program << " add <title>                     Add a new note (reads content from stdin)\n"
              << "  " << program << " edit <id> <title>               Update a note (reads content from stdin)\n"
              << "  " << program << " remove <id>                     Remove a note\n"
              << "\n  Reminders:\n"
              << "  " << program << " remind <title> <date>           Add a reminder (reads content from stdin)\n"
              << "  " << program << " reminders                       List all reminders\n"
              << "  " << program << " show-reminder <id>              Show a reminder\n"
              << "  " << program << " edit-reminder <id> <title> <date>  Update a reminder\n"
              << "  " << program << " remove-reminder <id>            Remove a reminder\n"
              << "\n  Date format: YYYY-MM-DD\n";
}

template <NoteRepositoryConcept<NoteType> NoteRepo, NoteRepositoryConcept<ReminderType> ReminderRepo>
int run(NoteRepo& note_repo, ReminderRepo& reminder_repo, const std::vector<std::string>& args) {
    if (args.empty()) {
        return -1;
    }

    const auto& cmd = args[0];

    // --- Notes ---

    if (cmd == "list") {
        for (const auto& note : note_repo.list()) {
            std::cout << note.id().to_string() << "\t" << note.title() << "\n";
        }
        return 0;
    }

    if (cmd == "show") {
        if (args.size() < 2) {
            std::cerr << "Error: 'show' requires an id\n";
            return 1;
        }
        auto note = note_repo.get(IntegerId{std::stoi(args[1])});
        if (!note) {
            std::cerr << "Error: note '" << args[1] << "' not found\n";
            return 1;
        }
        std::cout << "Id:      " << note->id().to_string() << "\n"
                  << "Title:   " << note->title() << "\n"
                  << "Created: " << std::format("{:%FT%TZ}", note->created_at()) << "\n"
                  << "Updated: " << std::format("{:%FT%TZ}", note->updated_at()) << "\n"
                  << "\n" << note->content() << "\n";
        return 0;
    }

    if (cmd == "add") {
        if (args.size() < 2) {
            std::cerr << "Error: 'add' requires <title>\n";
            return 1;
        }
        std::cerr << eof_hint << "\n";
        std::string content((std::istreambuf_iterator<char>(std::cin)),
                             std::istreambuf_iterator<char>());
        auto id = note_repo.next_id();
        auto note = NoteType::CreateNew(id, args[1], std::move(content));
        note_repo.save(note);
        std::cout << "Added note '" << id.to_string() << "'\n";
        return 0;
    }

    if (cmd == "edit") {
        if (args.size() < 3) {
            std::cerr << "Error: 'edit' requires <id> and <title>\n";
            return 1;
        }
        auto existing = note_repo.get(IntegerId{std::stoi(args[1])});
        if (!existing) {
            std::cerr << "Error: note '" << args[1] << "' not found\n";
            return 1;
        }
        std::cerr << eof_hint << "\n";
        std::string content((std::istreambuf_iterator<char>(std::cin)),
                             std::istreambuf_iterator<char>());
        auto updated = existing->with_updated_title(args[2])
                                .with_updated_content(std::move(content));
        note_repo.save(updated);
        std::cout << "Updated note '" << args[1] << "'\n";
        return 0;
    }

    if (cmd == "remove") {
        if (args.size() < 2) {
            std::cerr << "Error: 'remove' requires an id\n";
            return 1;
        }
        note_repo.remove(IntegerId{std::stoi(args[1])});
        std::cout << "Removed note '" << args[1] << "'\n";
        return 0;
    }

    // --- Reminders ---

    if (cmd == "remind") {
        if (args.size() < 3) {
            std::cerr << "Error: 'remind' requires <title> and <date>\n";
            return 1;
        }
        auto remind_at = parse_date(args[2]);
        std::cerr << eof_hint << "\n";
        std::string content((std::istreambuf_iterator<char>(std::cin)),
                             std::istreambuf_iterator<char>());
        auto id = reminder_repo.next_id();
        auto reminder = ReminderType::CreateNew(id, args[1], std::move(content), remind_at);
        reminder_repo.save(reminder);
        std::cout << "Added reminder '" << id.to_string() << "'\n";
        return 0;
    }

    if (cmd == "reminders") {
        for (const auto& r : reminder_repo.list()) {
            std::cout << r.id().to_string() << "\t"
                      << std::format("{:%F}", r.remind_at()) << "\t"
                      << r.title() << "\n";
        }
        return 0;
    }

    if (cmd == "show-reminder") {
        if (args.size() < 2) {
            std::cerr << "Error: 'show-reminder' requires an id\n";
            return 1;
        }
        auto r = reminder_repo.get(IntegerId{std::stoi(args[1])});
        if (!r) {
            std::cerr << "Error: reminder '" << args[1] << "' not found\n";
            return 1;
        }
        std::cout << "Id:       " << r->id().to_string() << "\n"
                  << "Title:    " << r->title() << "\n"
                  << "Remind:   " << std::format("{:%F}", r->remind_at()) << "\n"
                  << "Created:  " << std::format("{:%FT%TZ}", r->created_at()) << "\n"
                  << "Updated:  " << std::format("{:%FT%TZ}", r->updated_at()) << "\n"
                  << "\n" << r->content() << "\n";
        return 0;
    }

    if (cmd == "edit-reminder") {
        if (args.size() < 4) {
            std::cerr << "Error: 'edit-reminder' requires <id>, <title>, and <date>\n";
            return 1;
        }
        auto existing = reminder_repo.get(IntegerId{std::stoi(args[1])});
        if (!existing) {
            std::cerr << "Error: reminder '" << args[1] << "' not found\n";
            return 1;
        }
        auto remind_at = parse_date(args[3]);
        std::cerr << eof_hint << "\n";
        std::string content((std::istreambuf_iterator<char>(std::cin)),
                             std::istreambuf_iterator<char>());
        auto updated = existing->with_updated_title(args[2])
                                .with_updated_remind_at(remind_at)
                                .with_updated_content(std::move(content));
        reminder_repo.save(updated);
        std::cout << "Updated reminder '" << args[1] << "'\n";
        return 0;
    }

    if (cmd == "remove-reminder") {
        if (args.size() < 2) {
            std::cerr << "Error: 'remove-reminder' requires an id\n";
            return 1;
        }
        reminder_repo.remove(IntegerId{std::stoi(args[1])});
        std::cout << "Removed reminder '" << args[1] << "'\n";
        return 0;
    }

    std::cerr << "Unknown command: " << cmd << "\n";
    print_usage("notese");
    return 1;
}

} // namespace notes::cli
