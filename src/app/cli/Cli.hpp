    #pragma once
#include <iostream>
#include <iterator>
#include <string>
#include <vector>
#include <sstream>
#include <chrono>
#include <functional>
#include <format>
#include <unordered_map>
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

inline std::chrono::system_clock::time_point parse_date(const std::string& s) {
    using Clock = std::chrono::system_clock;

    std::chrono::sys_seconds tp{};
    std::istringstream iss(s);
    std::chrono::from_stream(iss, "%Y-%m-%d", tp);
    if (iss.fail()) {
        iss.clear();
        iss.str(s);
        std::chrono::from_stream(iss, "%Y-%m-%dT%H:%M:%SZ", tp);
    }
    if (iss.fail()) {
        return Clock::time_point{};
    }
    return std::chrono::time_point_cast<Clock::duration>(tp);
}

inline std::string read_stdin_content() {
    std::cerr << eof_hint << "\n";
    return std::string((std::istreambuf_iterator<char>(std::cin)),
                       std::istreambuf_iterator<char>());
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

    using CommandHandler = std::function<int(const std::vector<std::string>&)>;
    std::unordered_map<std::string, CommandHandler> commands;

    commands.emplace("list", [&](const std::vector<std::string>& command_args) -> int {
        (void)command_args;
        for (const auto& note : note_repo.list()) {
            std::cout << note.id().to_string() << "\t" << note.title() << "\n";
        }
        return 0;
    });

    commands.emplace("show", [&](const std::vector<std::string>& command_args) -> int {
        if (command_args.size() < 2) {
            std::cerr << "Error: 'show' requires an id\n";
            return 1;
        }
        auto note = note_repo.get(IntegerId{std::stoi(command_args[1])});
        if (!note) {
            std::cerr << "Error: note '" << command_args[1] << "' not found\n";
            return 1;
        }
        std::cout << "Id:      " << note->id().to_string() << "\n"
                  << "Title:   " << note->title() << "\n"
                  << "Created: " << std::format("{:%FT%TZ}", note->created_at()) << "\n"
                  << "Updated: " << std::format("{:%FT%TZ}", note->updated_at()) << "\n"
                  << "\n" << note->content() << "\n";
        return 0;
    });

    commands.emplace("add", [&](const std::vector<std::string>& command_args) -> int {
        if (command_args.size() < 2) {
            std::cerr << "Error: 'add' requires <title>\n";
            return 1;
        }
        std::string content = read_stdin_content();
        auto id = note_repo.next_id();
        auto note = NoteType::CreateNew(id, command_args[1], std::move(content));
        note_repo.save(note);
        std::cout << "Added note '" << id.to_string() << "'\n";
        return 0;
    });

    commands.emplace("edit", [&](const std::vector<std::string>& command_args) -> int {
        if (command_args.size() < 3) {
            std::cerr << "Error: 'edit' requires <id> and <title>\n";
            return 1;
        }
        auto existing = note_repo.get(IntegerId{std::stoi(command_args[1])});
        if (!existing) {
            std::cerr << "Error: note '" << command_args[1] << "' not found\n";
            return 1;
        }
        std::string content = read_stdin_content();
        auto updated = existing->with_updated_title(command_args[2])
                                .with_updated_content(std::move(content));
        note_repo.save(updated);
        std::cout << "Updated note '" << command_args[1] << "'\n";
        return 0;
    });

    commands.emplace("remove", [&](const std::vector<std::string>& command_args) -> int {
        if (command_args.size() < 2) {
            std::cerr << "Error: 'remove' requires an id\n";
            return 1;
        }
        note_repo.remove(IntegerId{std::stoi(command_args[1])});
        std::cout << "Removed note '" << command_args[1] << "'\n";
        return 0;
    });

    commands.emplace("remind", [&](const std::vector<std::string>& command_args) -> int {
        if (command_args.size() < 3) {
            std::cerr << "Error: 'remind' requires <title> and <date>\n";
            return 1;
        }
        auto remind_at = parse_date(command_args[2]);
        std::string content = read_stdin_content();
        auto id = reminder_repo.next_id();
        auto reminder = ReminderType::CreateNew(id, command_args[1], std::move(content), remind_at);
        reminder_repo.save(reminder);
        std::cout << "Added reminder '" << id.to_string() << "'\n";
        return 0;
    });

    commands.emplace("reminders", [&](const std::vector<std::string>& command_args) -> int {
        (void)command_args;
        for (const auto& r : reminder_repo.list()) {
            std::cout << r.id().to_string() << "\t"
                      << std::format("{:%F}", r.remind_at()) << "\t"
                      << r.title() << "\n";
        }
        return 0;
    });

    commands.emplace("show-reminder", [&](const std::vector<std::string>& command_args) -> int {
        if (command_args.size() < 2) {
            std::cerr << "Error: 'show-reminder' requires an id\n";
            return 1;
        }
        auto r = reminder_repo.get(IntegerId{std::stoi(command_args[1])});
        if (!r) {
            std::cerr << "Error: reminder '" << command_args[1] << "' not found\n";
            return 1;
        }
        std::cout << "Id:       " << r->id().to_string() << "\n"
                  << "Title:    " << r->title() << "\n"
                  << "Remind:   " << std::format("{:%F}", r->remind_at()) << "\n"
                  << "Created:  " << std::format("{:%FT%TZ}", r->created_at()) << "\n"
                  << "Updated:  " << std::format("{:%FT%TZ}", r->updated_at()) << "\n"
                  << "\n" << r->content() << "\n";
        return 0;
    });

    commands.emplace("edit-reminder", [&](const std::vector<std::string>& command_args) -> int {
        if (command_args.size() < 4) {
            std::cerr << "Error: 'edit-reminder' requires <id>, <title>, and <date>\n";
            return 1;
        }
        auto existing = reminder_repo.get(IntegerId{std::stoi(command_args[1])});
        if (!existing) {
            std::cerr << "Error: reminder '" << command_args[1] << "' not found\n";
            return 1;
        }
        auto remind_at = parse_date(command_args[3]);
        std::string content = read_stdin_content();
        auto updated = existing->with_updated_title(command_args[2])
                                .with_updated_remind_at(remind_at)
                                .with_updated_content(std::move(content));
        reminder_repo.save(updated);
        std::cout << "Updated reminder '" << command_args[1] << "'\n";
        return 0;
    });

    commands.emplace("remove-reminder", [&](const std::vector<std::string>& command_args) -> int {
        if (command_args.size() < 2) {
            std::cerr << "Error: 'remove-reminder' requires an id\n";
            return 1;
        }
        reminder_repo.remove(IntegerId{std::stoi(command_args[1])});
        std::cout << "Removed reminder '" << command_args[1] << "'\n";
        return 0;
    });

    if (auto it = commands.find(cmd); it != commands.end()) {
        return it->second(args);
    }

    std::cerr << "Unknown command: " << cmd << "\n";
    print_usage("notese");
    return 1;
}

} // namespace notes::cli
