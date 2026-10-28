add_library(io_dev_memory STATIC io_dev_memory.c)
target_link_libraries(io_dev_memory PUBLIC io_dev)
target_link_libraries(io_dev_memory PRIVATE helper)
target_include_directories(io_dev_memory PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
