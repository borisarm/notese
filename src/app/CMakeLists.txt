add_executable(notese main.cpp)

target_include_directories(notese PRIVATE
    cli
    tui)

target_link_libraries(notese PRIVATE
    notese_domain
    notese_infrastructure
    ftxui::dom
    ftxui::screen
    ftxui::component)
