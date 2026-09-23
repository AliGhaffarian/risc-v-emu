add_library(cpu STATIC cpu.c)
target_link_libraries(cpu PRIVATE helper)
target_include_directories(cpu PRIVATE ${CMAKE_SOURCE_DIR}/src/helper)
