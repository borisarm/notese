add_library(notese_domain)

target_sources(notese_domain PRIVATE src/Note.cpp)

target_include_directories(notese_domain PUBLIC include)
