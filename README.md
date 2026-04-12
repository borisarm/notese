# notese

A minimal note-taking app that stores notes as Markdown files. Run it without arguments for a fullscreen TUI, or pass commands for quick CLI usage.

## Features

- **TUI mode** — fullscreen terminal UI with note list and detail view (powered by [FTXUI](https://github.com/ArthurSonzogni/FTXUI))
- **CLI mode** — scriptable commands for listing, viewing, adding, editing, and removing notes
- **Markdown storage** — notes are saved as `.md` files with YAML front matter in `~/.notese/`
- **Modern C++23** — concepts, `<=>`, `std::format`, `std::chrono::parse`

## Usage

```
notese                         Launch TUI
notese list                    List all notes
notese show <id>               Show a note
notese add <id> <title>        Add a new note (reads content from stdin)
notese edit <id> <title>       Update a note (reads content from stdin)
notese remove <id>             Remove a note
```

### Examples

```bash
# Add a note
echo "Remember to buy milk" | notese add groceries "Grocery List"

# List notes
notese list

# Show a note
notese show groceries

# Edit a note
echo "Updated content" | notese edit groceries "Grocery List"

# Remove a note
notese remove groceries

# Launch the TUI
notese
```

## Building

### Prerequisites

- CMake 4.2+
- A C++23 compiler (Clang 21+, GCC 15+)
- [vcpkg](https://vcpkg.io/)

### Build

```bash
cmake -B build
cmake --build build
```

The binary is at `build/src/app/notese`.

### Run tests

```bash
ctest --test-dir build --output-on-failure
```

## Project Structure

```
src/
  domain/          Core types: Note, StringId, concepts
  infrastructure/  Repositories: InMemoryNoteRepository, MarkdownNoteRepository
  app/             Single binary with CLI and TUI modes
tests/
  domain/          Note and StringId tests
  infrastructure/  Repository tests
```

## License

[MIT](LICENSE) © Boris Armenta
