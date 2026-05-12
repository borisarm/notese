#pragma once

#include <initializer_list>
#include <utility>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>

namespace notes::tui {

class AppRuntime {
public:
    template <typename RenderFn, typename EventFn>
    static void run(ftxui::ScreenInteractive& screen,
                    std::initializer_list<ftxui::Component> stacked_components,
                    RenderFn&& render_fn,
                    EventFn&& event_fn) {
        auto main_component = ftxui::Renderer(
            ftxui::Container::Stacked(stacked_components),
            std::forward<RenderFn>(render_fn));

        auto with_events = ftxui::CatchEvent(
            main_component,
            std::forward<EventFn>(event_fn));

        screen.Loop(with_events);
    }
};

} // namespace notes::tui