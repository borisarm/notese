#pragma once

#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>

namespace notes::tui {

inline auto parse_date(const std::string& s) {
    using Clock = std::chrono::system_clock;
    using TimePoint = std::chrono::time_point<Clock>;
    TimePoint tp{};
    std::istringstream iss(s);
    std::tm tm{};
    iss >> std::get_time(&tm, "%Y-%m-%d");
    if (!iss.fail()) {
        auto sctp = std::chrono::system_clock::from_time_t(std::mktime(&tm));
        tp = std::chrono::time_point_cast<Clock::duration>(sctp);
    }
    return tp;
}

} // namespace notes::tui
