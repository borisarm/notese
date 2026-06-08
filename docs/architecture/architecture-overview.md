# Architecture Overview

## Summary

notese follows a layered architecture with clear separation between:

- Domain: immutable business objects and generic contracts.
- Infrastructure: repository implementations for persistence.
- Application: delivery layer (CLI and TUI) that orchestrates user interaction and repository calls.

The executable composes repositories in `main` and dispatches either to CLI (when arguments are provided) or to TUI (when no arguments are provided).

## Layers and Responsibilities

### Domain layer

Primary files:

- `src/domain/include/Note.hpp`
- `src/domain/include/Reminder.hpp`
- `src/domain/include/IntegerId.hpp`
- `src/domain/include/StringId.hpp`
- `src/domain/include/NoteIdConcept.hpp`
- `src/domain/include/NoteRepositoryConcept.hpp`

Responsibilities:

- Defines core entities (`Note`, `Reminder`) as immutable value-oriented templates.
- Defines id contracts (`NoteIdConcept`) and concrete id types (`IntegerId`, `StringId`).
- Defines repository behavior contract (`NoteRepositoryConcept`) using C++ concepts.

Key design note:

- `Note` and `Reminder` expose functional update methods (for example `with_updated_title`) that return new instances instead of mutating state.

### Infrastructure layer

Primary files:

- `src/infrastructure/include/InMemoryNoteRepository.hpp`
- `src/infrastructure/include/MarkdownNoteRepository.hpp`
- `src/infrastructure/include/MarkdownReminderRepository.hpp`

Responsibilities:

- Implements repository operations (`save`, `get`, `list`, `remove`, `next_id`).
- Persists notes/reminders as Markdown files with YAML front matter.
- Parses and formats date-time values to and from ISO-8601 strings.
- Provides in-memory repository variant for tests and non-persistent scenarios.

Storage format:

- One Markdown file per aggregate under `~/.notese/notes` or `~/.notese/reminders`.
- File name is derived from id (for example `12.md`).

### Application layer

Primary files:

- `src/app/main.cpp`
- `src/app/cli/Cli.hpp`
- `src/app/tui/Tui.hpp`

Responsibilities:

- Builds repository instances and determines runtime mode.
- CLI path: command parsing and script-friendly input/output.
- TUI path: interactive browsing/editing using FTXUI.

## Runtime Composition

`main` composes concrete repositories:

- `MarkdownNoteRepository<Note<IntegerId>>`
- `MarkdownReminderRepository<Reminder<IntegerId>>`

Then:

1. Calls `notes::cli::run(...)` with command line args.
2. If CLI returns `-1` (no command), runs `notes::tui::run(...)`.
3. Returns status code from selected mode.

## Build-Time Architecture

CMake modules:

- `src/domain` builds `notese_domain`.
- `src/infrastructure` builds `notese_infrastructure`.
- `src/app` builds executable `notese`.
- `tests/domain` and `tests/infrastructure` build separate GTest binaries.

The codebase is template-heavy, so many core behaviors are header-defined and instantiated by the app/tests.

## Quality and Verification Strategy

- Domain tests validate id semantics, ordering, equality, and immutable update behavior.
- Infrastructure tests validate persistence roundtrips, missing-item behavior, list/remove behavior, and id generation.
- Concept checks (`static_assert`) enforce repository contract conformance at compile time.

## Architectural Strengths

- Clear separation of concerns by layer.
- Strong type-level contracts with concepts.
- Easy replacement of repository implementations.
- Value semantics reduce mutable shared-state complexity.

## Current Limitations

- The same repository concept is reused for notes and reminders; this is practical but semantically broad.
- Some infrastructure placeholders are present but empty (`FileNoteRepository`, `JsonSerializer`, `SearchService`).
- Date parsing in app layer accepts limited formats and has no validation feedback beyond default/zero timepoint behavior.
