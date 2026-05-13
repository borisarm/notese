cmake_minimum_required(VERSION 4.2)
project(notese)
set(CMAKE_CXX_STANDARD 23)

option(ENABLE_COVERAGE "Build with gcov instrumentation (--coverage)" OFF)

if(ENABLE_COVERAGE)
    if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        message(FATAL_ERROR "ENABLE_COVERAGE requires GCC or Clang (got ${CMAKE_CXX_COMPILER_ID}).")
    endif()
    add_compile_options(--coverage -O0 -g)
    add_link_options(--coverage)
endif()

find_package(ftxui CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)

enable_testing()

add_subdirectory(src)
add_subdirectory(tests)

if(ENABLE_COVERAGE)
    find_program(GCOVR_EXECUTABLE gcovr)
    if(NOT GCOVR_EXECUTABLE)
        message(WARNING "gcovr not found in PATH; the 'coverage' target will not be available. Install with: pipx install gcovr  (or: sudo apt install gcovr)")
    else()
        # Pick a gcov tool matching the compiler version when possible.
        set(_gcov_tool "gcov")
        if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            string(REGEX MATCH "^[0-9]+" _gcc_major "${CMAKE_CXX_COMPILER_VERSION}")
            find_program(_gcov_versioned "gcov-${_gcc_major}")
            if(_gcov_versioned)
                set(_gcov_tool "${_gcov_versioned}")
            endif()
        endif()

        set(_cov_dir "${CMAKE_BINARY_DIR}/coverage")
        file(MAKE_DIRECTORY "${_cov_dir}")

        add_custom_target(coverage
            COMMAND ${CMAKE_COMMAND} -E make_directory "${_cov_dir}"
            # Reset counters so the report reflects only this run.
            COMMAND ${GCOVR_EXECUTABLE}
                    --root "${CMAKE_SOURCE_DIR}"
                    --gcov-executable "${_gcov_tool}"
                    --delete
                    --filter "${CMAKE_SOURCE_DIR}/src"
                    # Exclude entry-point/runtime files that require an
                    # interactive terminal and therefore can't be exercised
                    # by the headless unit-test suite.
                    --exclude "${CMAKE_SOURCE_DIR}/src/app/main\\.cpp"
                    --exclude "${CMAKE_SOURCE_DIR}/src/app/tui/Tui\\.hpp"
                    --exclude "${CMAKE_SOURCE_DIR}/src/app/tui/TuiController\\.hpp"
                    --exclude "${CMAKE_SOURCE_DIR}/src/app/tui/AppRuntime\\.hpp"
                    --exclude "${CMAKE_SOURCE_DIR}/src/app/tui/TuiComponents\\.hpp"
                    --exclude-throw-branches
                    --exclude-unreachable-branches
                    --print-summary
                    --html-details "${_cov_dir}/index.html"
                    --xml "${_cov_dir}/coverage.xml"
                    --txt "${_cov_dir}/coverage.txt"
                    "${CMAKE_BINARY_DIR}"
            WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
            COMMENT "Generating coverage report in ${_cov_dir}"
            VERBATIM
            USES_TERMINAL
        )

        # Convenience: build, run tests, then produce the report.
        add_custom_target(coverage-run
            COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
            COMMAND ${CMAKE_COMMAND} --build "${CMAKE_BINARY_DIR}" --target coverage
            WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
            COMMENT "Running tests then generating coverage report"
            USES_TERMINAL
        )
    endif()
endif()