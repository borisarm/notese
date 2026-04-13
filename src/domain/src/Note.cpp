#include <string>
#include "Note.hpp"
#include "StringId.hpp"
#include "IntegerId.hpp"

static notes::Note<notes::StringId> __some{{""},  "", "", std::chrono::system_clock::now(), std::chrono::system_clock::now()};
static notes::Note<notes::IntegerId> __some2{{0}, "", "", std::chrono::system_clock::now(), std::chrono::system_clock::now()};