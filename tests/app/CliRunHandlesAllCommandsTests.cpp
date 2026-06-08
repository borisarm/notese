#include <gtest/gtest.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "Cli.hpp"
#include "InMemoryNoteRepository.hpp"
#include "IntegerId.hpp"
#include "Note.hpp"
#include "Reminder.hpp"

namespace notes::tests {

namespace {

using CliNote = notes::Note<notes::IntegerId>;
using CliReminder = notes::Reminder<notes::IntegerId>;
using CliNoteRepo = InMemoryNoteRepository<CliNote>;
using CliReminderRepo = InMemoryNoteRepository<CliReminder>;

class CliFixture : public ::testing::Test {
protected:
    CliNoteRepo note_repo;
    CliReminderRepo reminder_repo;

    std::ostringstream out;
    std::ostringstream err;
    std::istringstream in;

    std::streambuf* original_cout = nullptr;
    std::streambuf* original_cerr = nullptr;
    std::streambuf* original_cin = nullptr;

    void SetUp() override {
        original_cout = std::cout.rdbuf();
        original_cerr = std::cerr.rdbuf();
        original_cin = std::cin.rdbuf();
    }

    void redirect(const std::string& stdin_content = "") {
        in.str(stdin_content);
        in.clear();
        std::cout.rdbuf(out.rdbuf());
        std::cerr.rdbuf(err.rdbuf());
        std::cin.rdbuf(in.rdbuf());
    }

    void TearDown() override {
        if (original_cout) std::cout.rdbuf(original_cout);
        if (original_cerr) std::cerr.rdbuf(original_cerr);
        if (original_cin) std::cin.rdbuf(original_cin);
    }

    int run(std::vector<std::string> args, const std::string& stdin_content = "") {
        redirect(stdin_content);
        return notes::cli::run(note_repo, reminder_repo, args);
    }
};

} // namespace

TEST_F(CliFixture, EmptyArgsReturnsMinusOneSignalingTuiLaunch) {
    redirect();
    EXPECT_EQ(notes::cli::run(note_repo, reminder_repo, {}), -1);
}

TEST_F(CliFixture, UnknownCommandPrintsUsageAndReturnsOne) {
    EXPECT_EQ(run({"bogus-command"}), 1);
    EXPECT_NE(err.str().find("Unknown command"), std::string::npos);
    EXPECT_NE(err.str().find("Usage:"), std::string::npos);
}

TEST_F(CliFixture, AddListShowEditRemoveNoteRoundTrip) {
    EXPECT_EQ(run({"add", "first-note"}, "hello world"), 0);
    EXPECT_NE(out.str().find("Added note"), std::string::npos);

    out.str("");
    EXPECT_EQ(run({"list"}), 0);
    EXPECT_NE(out.str().find("first-note"), std::string::npos);

    out.str("");
    EXPECT_EQ(run({"show", "1"}), 0);
    EXPECT_NE(out.str().find("first-note"), std::string::npos);
    EXPECT_NE(out.str().find("hello world"), std::string::npos);

    out.str("");
    EXPECT_EQ(run({"edit", "1", "renamed"}, "updated content"), 0);
    EXPECT_NE(out.str().find("Updated note"), std::string::npos);

    out.str("");
    EXPECT_EQ(run({"show", "1"}), 0);
    EXPECT_NE(out.str().find("renamed"), std::string::npos);
    EXPECT_NE(out.str().find("updated content"), std::string::npos);

    out.str("");
    EXPECT_EQ(run({"remove", "1"}), 0);
    EXPECT_NE(out.str().find("Removed note"), std::string::npos);
    EXPECT_FALSE(note_repo.get(notes::IntegerId{1}).has_value());
}

TEST_F(CliFixture, NoteCommandsReportMissingArgumentErrors) {
    EXPECT_EQ(run({"show"}), 1);
    EXPECT_NE(err.str().find("requires an id"), std::string::npos);
    err.str("");

    EXPECT_EQ(run({"add"}), 1);
    EXPECT_NE(err.str().find("requires <title>"), std::string::npos);
    err.str("");

    EXPECT_EQ(run({"edit", "1"}), 1);
    EXPECT_NE(err.str().find("requires <id> and <title>"), std::string::npos);
    err.str("");

    EXPECT_EQ(run({"remove"}), 1);
    EXPECT_NE(err.str().find("requires an id"), std::string::npos);
}

TEST_F(CliFixture, ShowAndEditAndRemoveReportMissingNote) {
    EXPECT_EQ(run({"show", "999"}), 1);
    EXPECT_NE(err.str().find("not found"), std::string::npos);
    err.str("");

    EXPECT_EQ(run({"edit", "999", "title"}, "content"), 1);
    EXPECT_NE(err.str().find("not found"), std::string::npos);
}

TEST_F(CliFixture, AddListShowEditRemoveReminderRoundTrip) {
    EXPECT_EQ(run({"remind", "doctor", "2030-05-13"}, "appointment"), 0);
    EXPECT_NE(out.str().find("Added reminder"), std::string::npos);

    out.str("");
    EXPECT_EQ(run({"reminders"}), 0);
    EXPECT_NE(out.str().find("doctor"), std::string::npos);
    EXPECT_NE(out.str().find("2030-05-13"), std::string::npos);

    out.str("");
    EXPECT_EQ(run({"show-reminder", "1"}), 0);
    EXPECT_NE(out.str().find("doctor"), std::string::npos);
    EXPECT_NE(out.str().find("appointment"), std::string::npos);

    out.str("");
    EXPECT_EQ(run({"edit-reminder", "1", "dentist", "2031-06-14"}, "rescheduled"), 0);
    EXPECT_NE(out.str().find("Updated reminder"), std::string::npos);

    out.str("");
    EXPECT_EQ(run({"show-reminder", "1"}), 0);
    EXPECT_NE(out.str().find("dentist"), std::string::npos);
    EXPECT_NE(out.str().find("2031-06-14"), std::string::npos);
    EXPECT_NE(out.str().find("rescheduled"), std::string::npos);

    out.str("");
    EXPECT_EQ(run({"remove-reminder", "1"}), 0);
    EXPECT_NE(out.str().find("Removed reminder"), std::string::npos);
    EXPECT_FALSE(reminder_repo.get(notes::IntegerId{1}).has_value());
}

TEST_F(CliFixture, ReminderCommandsReportMissingArgumentErrors) {
    EXPECT_EQ(run({"remind", "title-only"}), 1);
    EXPECT_NE(err.str().find("requires <title> and <date>"), std::string::npos);
    err.str("");

    EXPECT_EQ(run({"show-reminder"}), 1);
    EXPECT_NE(err.str().find("requires an id"), std::string::npos);
    err.str("");

    EXPECT_EQ(run({"edit-reminder", "1", "title"}), 1);
    EXPECT_NE(err.str().find("requires <id>, <title>, and <date>"), std::string::npos);
    err.str("");

    EXPECT_EQ(run({"remove-reminder"}), 1);
    EXPECT_NE(err.str().find("requires an id"), std::string::npos);
    err.str("");

    EXPECT_EQ(run({"show-reminder", "999"}), 1);
    EXPECT_NE(err.str().find("not found"), std::string::npos);
    err.str("");

    EXPECT_EQ(run({"edit-reminder", "999", "x", "2030-01-01"}, "content"), 1);
    EXPECT_NE(err.str().find("not found"), std::string::npos);
}

TEST_F(CliFixture, ParseDateAcceptsPlainDateAndIso8601AndRejectsGarbage) {
    using notes::cli::parse_date;
    using Clock = std::chrono::system_clock;

    EXPECT_NE(parse_date("2030-05-13"), Clock::time_point{});
    EXPECT_NE(parse_date("2030-05-13T12:34:56Z"), Clock::time_point{});
    EXPECT_EQ(parse_date("not-a-date"), Clock::time_point{});
}

} // namespace notes::tests
