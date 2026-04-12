#include <string>
#include "Note.hpp"
#include "StringId.hpp"

static notes::Note<notes::StringId> __some{{""}, "", "", std::chrono::system_clock::now(), std::chrono::system_clock::now()};