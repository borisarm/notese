add_executable(notese main.cpp)

target_include_directories(notese PRIVATE
    cli
    tui
    ${CMAKE_SOURCE_DIR}/src/domain/include
    ${CMAKE_SOURCE_DIR}/src/infrastructure/include)

target_link_libraries(notese PRIVATE
    ftxui::dom
    ftxui::screen
    ftxui::component)
