add_library(cpu STATIC cpu.c)
target_link_libraries(cpu PUBLIC sysbus helper)
target_include_directories(cpu PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
