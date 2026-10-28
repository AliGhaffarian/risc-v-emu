add_library(io_dev io_dev.c)
target_include_directories(io_dev PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(io_dev PRIVATE helper)
add_subdirectory(io_dev_memory)
