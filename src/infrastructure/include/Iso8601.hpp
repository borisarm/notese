#pragma once

#include <charconv>
#include <chrono>
#include <cstddef>
#include <format>
#include <string>
#include <string_view>
#include <system_error>

namespace notes::infra {

template <typename TimePoint>
inline std::string to_iso8601(TimePoint tp) {
    return std::format("{:%FT%TZ}", tp);
}

template <typename TimePoint>
inline TimePoint from_iso8601(const std::string& s) {
    using Clock = typename TimePoint::clock;

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

} // namespace notes::infra
