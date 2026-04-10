cmake_minimum_required(VERSION 4.2)
project(notese)
set(CMAKE_CXX_STANDARD 23)

find_package(ftxui CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)

add_subdirectory(src)
add_subdirectory(tests)