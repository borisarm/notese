#pragma once

#include <algorithm>
#include "DateInputParser.hpp"
#include "FileWriter.hpp"
#include "Iso8601.hpp"
#include "NoteRepositoryConcept.hpp"
#include "TuiState.hpp"

namespace notes::tui {

template <NoteRepositoryConcept<ReminderType> ReminderRepo>
class ReminderWorkflowService {
public:
    explicit ReminderWorkflowService(ReminderRepo& reminder_repo)
        : reminder_repo_(reminder_repo) {}

    void refresh(TuiState& state) {
        state.reminders = reminder_repo_.list();
        std::sort(state.reminders.begin(), state.reminders.end(), [](const ReminderType& a, const ReminderType& b) {
            return a.remind_at() < b.remind_at();
        });

        state.reminder_entries.clear();
        for (const auto& reminder : state.reminders) {
            state.reminder_entries.push_back(
                reminder.id().to_string() + "  " + infra::to_utc_date(reminder.remind_at()) + "  " + reminder.title());
        }

        if (state.selection.reminder_selected >= (int)state.reminders.size()) {
            state.selection.reminder_selected = std::max(0, (int)state.reminders.size() - 1);
        }
    }

    void save_form(TuiState& state) {
        try {
            auto remind_at = parse_date(state.form.date);

            if (state.mode == Mode::AddReminder) {
                auto id = reminder_repo_.next_id();
                auto reminder = ReminderType::CreateNew(id, state.form.title, state.form.content, remind_at);
                reminder_repo_.save(reminder);
            } else if (!state.reminders.empty()) {
                int idx = std::clamp(state.selection.reminder_selected, 0, (int)state.reminders.size() - 1);
                auto updated = state.reminders[idx]
                    .with_updated_title(state.form.title)
                    .with_updated_remind_at(remind_at)
                    .with_updated_content(state.form.content);
                reminder_repo_.save(updated);
            }

            refresh(state);
            state.mode = Mode::Browse;
            state.status_message.clear();
        } catch (const infra::FileWriteError& error) {
            state.status_message = error.what();
        }
    }

    void delete_selected(TuiState& state) {
        try {
            if (!state.reminders.empty()) {
                int idx = std::clamp(state.selection.reminder_selected, 0, (int)state.reminders.size() - 1);
                reminder_repo_.remove(state.reminders[idx].id());
            }

            refresh(state);
            state.mode = Mode::Browse;
            state.status_message.clear();
        } catch (const infra::FileWriteError& error) {
            state.status_message = error.what();
        }
    }

private:
    ReminderRepo& reminder_repo_;
};

} // namespace notes::tui
