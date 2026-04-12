add_library(notese_infrastructure)

target_sources(notese_infrastructure PRIVATE src/infrastructure.cpp)

target_include_directories(notese_infrastructure PRIVATE include ../domain/include)
target_link_libraries(notese_infrastructure PRIVATE notese_domain)
