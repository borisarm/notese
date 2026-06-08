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

# Release-only size optimizations: LTO + -Os + dead-code stripping + symbol strip.
include(CheckIPOSupported)
check_ipo_supported(RESULT _notese_ipo_supported OUTPUT _notese_ipo_error)
if(_notese_ipo_supported)
    set_property(TARGET notese PROPERTY
        INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
endif()

target_compile_options(notese PRIVATE
    $<$<CONFIG:Release>:-Os -ffunction-sections -fdata-sections>)

if(APPLE)
    target_link_options(notese PRIVATE
        $<$<CONFIG:Release>:-Wl,-dead_strip>)
else()
    target_link_options(notese PRIVATE
        $<$<CONFIG:Release>:-Wl,--gc-sections>)
endif()
