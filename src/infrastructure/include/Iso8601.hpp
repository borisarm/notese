#pragma once

#include <charconv>
#include <chrono>
#include <cstddef>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>
#include <string_view>
#include <system_error>

namespace notes::infra {

namespace detail {

inline std::tm utc_tm_from_time_t(std::time_t time_value) {
    std::tm tm{};
#ifdef _WIN32
    gmtime_s(&tm, &time_value);
#else
    gmtime_r(&time_value, &tm);
#endif
    return tm;
}

template <typename TimePoint>
inline std::string format_utc(TimePoint tp, const char* pattern) {
    const std::time_t t = TimePoint::clock::to_time_t(tp);
    const std::tm tm = utc_tm_from_time_t(t);

    std::ostringstream oss;
    oss << std::put_time(&tm, pattern);
    return oss.str();
}

} // namespace detail

template <typename TimePoint>
inline std::string to_iso8601(TimePoint tp) {
    using namespace std::chrono;

    const auto since_epoch = tp.time_since_epoch();
    const auto whole_seconds = floor<seconds>(since_epoch);
    auto fractional = duration_cast<nanoseconds>(since_epoch - whole_seconds);
    if (fractional < nanoseconds{0}) {
        fractional += seconds{1};
    }

    std::string out = detail::format_utc(TimePoint{whole_seconds}, "%Y-%m-%dT%H:%M:%S");
    if (fractional.count() != 0) {
        long long value = fractional.count();
        int width = 9;
        while (width > 1 && (value % 10) == 0) {
            value /= 10;
            --width;
        }

        std::ostringstream frac;
        frac << std::setw(width) << std::setfill('0') << value;
        out.push_back('.');
        out += frac.str();
    }
    out.push_back('Z');
    return out;
}

template <typename TimePoint>
inline std::string to_utc_date(TimePoint tp) {
    return detail::format_utc(tp, "%Y-%m-%d");
}

template <typename TimePoint>
inline std::string to_utc_datetime(TimePoint tp) {
    return detail::format_utc(tp, "%Y-%m-%d %H:%M:%S");
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
