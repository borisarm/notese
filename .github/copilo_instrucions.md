# Notes Project — Copilot Instructions  
Modern C++20 CLI + TUI (FTXUI) Notes Application  
Domain‑Driven, Template‑Based, Concept‑Constrained

---

## 1. Project Philosophy

This project is built around **modern C++20**, with a strong emphasis on:

- **Domain purity**  
- **Static polymorphism via concepts**  
- **Zero inheritance in the domain layer**  
- **No factories or builders unless strictly necessary**  
- **Explicit constraints instead of implicit conventions**  
- **Testability and reproducibility**  
- **Minimal dependencies**  
- **Clear separation between domain, infrastructure, CLI, and TUI**

The goal is to produce a clean, maintainable, modular system that can evolve without breaking existing components.

---

## 2. High‑Level Architecture

```
src/
  domain/
    Note.hpp
    NoteIdConcept.hpp
    NoteRepositoryConcept.hpp

  infrastructure/
    MarkdownNoteRepository.hpp
    (future: JsonNoteRepository, SqliteNoteRepository, etc.)

  app/
    CLI/
      CLIParser.cpp
      Commands.cpp

    TUI/
      (FTXUI components)
      MainScreen.cpp
      NoteListView.cpp
      NotePreview.cpp
      SearchBox.cpp
      AppController.cpp

tests/
  domain/
  infrastructure/
  app/
```

### Layer Responsibilities

#### **Domain**
- Pure logic  
- No I/O  
- No filesystem  
- No UI  
- No external libraries  
- All types validated via **concepts**  
- `Note` is a template over the ID type  
- `NoteId` types must satisfy `NoteIdConcept`

#### **Infrastructure**
- Persistence (Markdown, JSON, SQLite…)  
- ID generation (UUID, incremental, etc.)  
- Filesystem access  
- Serialization/deserialization  
- Must satisfy `NoteRepositoryConcept`

#### **Application Layer**
- CLI commands  
- TUI (FTXUI)  
- Orchestration  
- No domain logic  
- No persistence logic  

---

## 3. Domain Model

### 3.1 `NoteIdConcept`

Every ID type must satisfy:

- Copyable  
- Comparable (`==`, `<=>`)  
- Convertible to string via `to_string()`  

```cpp
template <typename T>
concept NoteIdConcept =
    std::copyable<T> &&
    requires(const T& a, const T& b) {
        { a == b } -> std::convertible_to<bool>;
        { a <=> b } -> std::same_as<std::strong_ordering>;
        { a.to_string() } -> std::convertible_to<std::string>;
    };
```

### 3.2 `Note<NoteId>`

- Template over the ID type  
- Immutable value semantics  
- Functional update methods  
- Timestamps using `std::chrono`  
- No ID generation inside the domain  

```cpp
template <NoteIdConcept NoteId>
class Note {
public:
    using IdType = NoteId;
    using Clock = std::chrono::system_clock;
    using TimePoint = std::chrono::time_point<Clock>;
    ...
};
```

---

## 4. Repository Abstraction (Concept‑Based)

### 4.1 `NoteRepositoryConcept`

A repository must provide:

- `save(note)`  
- `remove(id)`  
- `get(id) -> optional<Note>`  
- `list() -> vector<Note>`  

```cpp
template <typename Repo, typename Note>
concept NoteRepositoryConcept =
    requires(Repo repo, const Repo crepo, const Note& note, const typename Note::IdType& id) {
        { repo.save(note) } -> std::same_as<void>;
        { repo.remove(id) } -> std::same_as<void>;
        { crepo.get(id) } -> std::same_as<std::optional<Note>>;
        { crepo.list() } -> std::same_as<std::vector<Note>>;
    };
```

### 4.2 Markdown Repository

- One `.md` file per note  
- YAML front‑matter  
- Markdown body  
- Uses `std::chrono::format` and `std::chrono::parse`  
- No C time APIs (`gmtime`, `timegm`)  

---

## 5. CLI Layer

The CLI provides commands such as:

- `notes add "text"`  
- `notes list`  
- `notes view <id>`  
- `notes rm <id>`  
- `notes search <query>`  
- `notes tui` (launch TUI)

Responsibilities:

- Parse arguments  
- Call domain + repository  
- Print results  
- No business logic  

---

## 6. TUI Layer (FTXUI)

The TUI is built with **FTXUI** and follows a **Model–View–Update** pattern:

### Components

- **SearchBox**  
- **NoteListView**  
- **NotePreview**  
- **MainScreen**  
- **AppController** (state + event dispatch)

### Behavior

- Live fuzzy search  
- Keyboard navigation  
- Split‑pane layout  
- Markdown preview  
- Reactive rendering  

---

## 7. ID Generation

ID generation is **not** part of the domain.

It may live in:

- CLI  
- Infrastructure  
- A small utility module  

As long as the generated type satisfies `NoteIdConcept`.

Examples:

- `StringId`  
- `UuidId`  
- `IntId` (tests)  

---

## 8. Testing Strategy

### Domain Tests
- Pure `Note` behavior  
- No filesystem  
- No mocks  
- Deterministic IDs  

### Repository Tests
- Use a temporary directory  
- Validate round‑trip serialization  
- Validate list, get, save, remove  

### Concept Tests
- `static_assert(NoteRepositoryConcept<Repo, Note>)`  
- Ensures compile‑time contract correctness  

### TUI Tests
- Snapshot tests (optional)  
- Model tests (preferred)  

---

## 9. Coding Conventions

- C++20 only  
- No raw pointers  
- No inheritance in domain  
- Prefer value semantics  
- Prefer `std::optional` over exceptions  
- Use `std::chrono` for all timestamps  
- Use `std::filesystem` for persistence  
- Keep modules small and cohesive  
- Avoid global state  

---

## 10. Future Extensions

- Tagging system  
- Note linking  
- Fuzzy search scoring improvements  
- Sync with cloud storage  
- Encryption of notes  
- Richer Markdown rendering in TUI  
- Plugin system for new repositories  

---

## 11. Summary

This project is a **modern C++20 notes application** with:

- A **template‑based domain model**  
- **Concept‑validated repositories**  
- **Markdown persistence**  
- **FTXUI TUI**  
- **Clean CLI**  
- **Full testability**  
- **Zero runtime polymorphism**  
- **Strong compile‑time guarantees**

Copilot should follow these conventions when generating code, tests, or new modules.
