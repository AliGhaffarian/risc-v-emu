add_library(io_dev io_dev.c)
target_include_directories(io_dev PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(io_dev_memory)
