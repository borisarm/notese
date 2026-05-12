#pragma once
#include <filesystem>
#include <fstream>
#include <optional>
#include <vector>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <charconv>
#include <string_view>
#include <format>
#include "Note.hpp"
#include "NoteRepositoryConcept.hpp"

namespace notes {

    template <typename Note>
    class MarkdownNoteRepository {
    public:
        using Id = typename Note::IdType;
        using Clock = typename Note::Clock;
        using TimePoint = typename Note::TimePoint;

        explicit MarkdownNoteRepository(std::filesystem::path directory)
            : dir_(std::move(directory)) 
        {
            std::filesystem::create_directories(dir_);
        }

        // -----------------------------------------
        // save(note)
        // -----------------------------------------
        void save(const Note& note) {
            auto path = file_path(note.id());
            std::ofstream out(path);

            out << "---\n";
            out << "id: " << note.id().to_string() << "\n";
            out << "title: " << note.title() << "\n";
            out << "created_at: " << to_iso(note.created_at()) << "\n";
            out << "updated_at: " << to_iso(note.updated_at()) << "\n";
            out << "---\n\n";
            out << note.content();
        }

        // -----------------------------------------
        // remove(id)
        // -----------------------------------------
        void remove(const Id& id) {
            auto path = file_path(id);
            std::filesystem::remove(path);
        }

        // -----------------------------------------
        // get(id)
        // -----------------------------------------
        std::optional<Note> get(const Id& id) const {
            auto path = file_path(id);
            if (!std::filesystem::exists(path))
                return std::nullopt;

            return load_from_file(path);
        }

        // -----------------------------------------
        // list()
        // -----------------------------------------
        std::vector<Note> list() const {
            std::vector<Note> notes;

            for (auto& entry : std::filesystem::directory_iterator(dir_)) {
                if (entry.is_regular_file() && entry.path().extension() == ".md") {
                    if (auto note = load_from_file(entry.path()))
                        notes.push_back(*note);
                }
            }

            return notes;
        }

        // -----------------------------------------
        // next_id()
        // -----------------------------------------
        Id next_id() const {
            int max_id = 0;
            for (auto& entry : std::filesystem::directory_iterator(dir_)) {
                if (entry.is_regular_file() && entry.path().extension() == ".md") {
                    try {
                        int id = std::stoi(entry.path().stem().string());
                        if (id > max_id) max_id = id;
                    } catch (...) {}
                }
            }
            return Id{max_id + 1};
        }

    private:
        std::filesystem::path dir_;

        std::filesystem::path file_path(const Id& id) const {
            return dir_ / (id.to_string() + ".md");
        }

        // -----------------------------------------
        // Parse Markdown file
        // -----------------------------------------
        std::optional<Note> load_from_file(const std::filesystem::path& path) const {
            std::ifstream in(path);
            if (!in)
                return std::nullopt;

            std::string line;
            std::string id, title, created, updated;
            std::string content;

            // Expect front-matter
            std::getline(in, line);
            if (line != "---")
                return std::nullopt;

            while (std::getline(in, line)) {
                if (line == "---")
                    break;

                auto pos = line.find(": ");
                if (pos == std::string::npos)
                    continue;

                auto key = line.substr(0, pos);
                auto value = line.substr(pos + 2);

                if (key == "id") id = value;
                else if (key == "title") title = value;
                else if (key == "created_at") created = value;
                else if (key == "updated_at") updated = value;
            }

            // Read content
            std::ostringstream oss;
            oss << in.rdbuf();
            content = oss.str();

            return Note(
                Id{id},
                title,
                content,
                from_iso(created),
                from_iso(updated)
            );
        }

        // -----------------------------------------
        // ISO8601 helpers
        // -----------------------------------------
        static std::string to_iso(TimePoint tp) {
            return std::format("{:%FT%TZ}", tp);
        }

        static TimePoint from_iso(const std::string& s) {
            auto parse_int = [](std::string_view text, int& out) -> bool {
                const auto* begin = text.data();
                const auto* end = begin + text.size();
                auto [ptr, ec] = std::from_chars(begin, end, out);
                return ec == std::errc{} && ptr == end;
            };

            const std::string_view v{s};
            if (v.size() < 20) {
                return {};
            }

            if (v[4] != '-' || v[7] != '-' || v[10] != 'T' || v[13] != ':' || v[16] != ':') {
                return {};
            }

            int year_value = 0, month_value = 0, day_value = 0;
            int hour = 0, minute = 0, second = 0;
            if (!parse_int(v.substr(0, 4), year_value) ||
                !parse_int(v.substr(5, 2), month_value) ||
                !parse_int(v.substr(8, 2), day_value) ||
                !parse_int(v.substr(11, 2), hour) ||
                !parse_int(v.substr(14, 2), minute) ||
                !parse_int(v.substr(17, 2), second)) {
                return {};
            }

            std::chrono::nanoseconds fractional{0};
            std::size_t i = 19;
            if (i < v.size() && v[i] == '.') {
                ++i;
                std::size_t frac_start = i;
                while (i < v.size() && v[i] >= '0' && v[i] <= '9') {
                    ++i;
                }
                if (i == frac_start) {
                    return {};
                }

                long long value = 0;
                std::size_t digits = 0;
                for (std::size_t j = frac_start; j < i && digits < 9; ++j, ++digits) {
                    value = value * 10 + (v[j] - '0');
                }
                while (digits < 9) {
                    value *= 10;
                    ++digits;
                }
                fractional = std::chrono::nanoseconds{value};
            }

            std::chrono::minutes offset{0};
            if (i >= v.size()) {
                return {};
            }

            if (v[i] == 'Z') {
                ++i;
            } else if (v[i] == '+' || v[i] == '-') {
                const bool is_negative = v[i] == '-';
                ++i;
                if (i + 5 > v.size() || v[i + 2] != ':') {
                    return {};
                }

                int off_hour = 0, off_minute = 0;
                if (!parse_int(v.substr(i, 2), off_hour) || !parse_int(v.substr(i + 3, 2), off_minute)) {
                    return {};
                }
                i += 5;

                offset = std::chrono::hours{off_hour} + std::chrono::minutes{off_minute};
                if (is_negative) {
                    offset = -offset;
                }
            } else {
                return {};
            }

            if (i != v.size()) {
                return {};
            }

            using namespace std::chrono;
            const auto ymd = std::chrono::year{year_value} /
                             std::chrono::month{static_cast<unsigned>(month_value)} /
                             std::chrono::day{static_cast<unsigned>(day_value)};
            if (!ymd.ok()) {
                return {};
            }

            const auto base = sys_days{ymd} + hours{hour} + minutes{minute} + seconds{second} + fractional;
            const auto utc = base - offset;
            return time_point_cast<typename Clock::duration>(utc);
        }

    };

} // namespace notes
