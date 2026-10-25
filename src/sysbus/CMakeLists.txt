add_library(sysbus STATIC sysbus.c)

target_link_libraries(sysbus PRIVATE helper)
target_link_libraries(sysbus PRIVATE io_dev)

target_include_directories(sysbus PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
