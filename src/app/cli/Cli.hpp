    #pragma once
#include <iostream>
#include <string>
#include <vector>
#include "Note.hpp"
#include "StringId.hpp"
#include "NoteRepositoryConcept.hpp"

namespace notes::cli {

using NoteType = Note<StringId>;

#ifdef _WIN32
constexpr const char* eof_hint = "Enter content (Ctrl+Z then Enter to finish):";
#else
constexpr const char* eof_hint = "Enter content (Ctrl+D to finish):";
#endif

inline void print_usage(const char* program) {
    std::cerr << "Usage:\n"
              << "  " << program << "                      Launch TUI\n"
              << "  " << program << " list                  List all notes\n"
              << "  " << program << " show <id>             Show a note\n"
              << "  " << program << " add <id> <title>      Add a new note (reads content from stdin)\n"
              << "  " << program << " edit <id> <title>     Update a note (reads content from stdin)\n"
              << "  " << program << " remove <id>           Remove a note\n";
}

template <NoteRepositoryConcept<NoteType> Repo>
int run(Repo& repo, const std::vector<std::string>& args) {
    if (args.empty()) {
        return -1; // Signal to caller: no args, launch TUI
    }

    const auto& cmd = args[0];

    if (cmd == "list") {
        for (const auto& note : repo.list()) {
            std::cout << note.id().to_string() << "\t" << note.title() << "\n";
        }
        return 0;
    }

    if (cmd == "show") {
        if (args.size() < 2) {
            std::cerr << "Error: 'show' requires an id\n";
            return 1;
        }
        auto note = repo.get(StringId{args[1]});
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
        if (args.size() < 3) {
            std::cerr << "Error: 'add' requires <id> and <title>\n";
            return 1;
        }
        std::cerr << eof_hint << "\n";
        std::string content((std::istreambuf_iterator<char>(std::cin)),
                             std::istreambuf_iterator<char>());
        auto note = NoteType::CreateNew(StringId{args[1]}, args[2], std::move(content));
        repo.save(note);
        std::cout << "Added note '" << args[1] << "'\n";
        return 0;
    }

    if (cmd == "edit") {
        if (args.size() < 3) {
            std::cerr << "Error: 'edit' requires <id> and <title>\n";
            return 1;
        }
        auto existing = repo.get(StringId{args[1]});
        if (!existing) {
            std::cerr << "Error: note '" << args[1] << "' not found\n";
            return 1;
        }
        std::cerr << eof_hint << "\n";
        std::string content((std::istreambuf_iterator<char>(std::cin)),
                             std::istreambuf_iterator<char>());
        auto updated = existing->with_updated_title(args[2])
                                .with_updated_content(std::move(content));
        repo.save(updated);
        std::cout << "Updated note '" << args[1] << "'\n";
        return 0;
    }

    if (cmd == "remove") {
        if (args.size() < 2) {
            std::cerr << "Error: 'remove' requires an id\n";
            return 1;
        }
        repo.remove(StringId{args[1]});
        std::cout << "Removed note '" << args[1] << "'\n";
        return 0;
    }

    std::cerr << "Unknown command: " << cmd << "\n";
    print_usage("notese");
    return 1;
}

} // namespace notes::cli
