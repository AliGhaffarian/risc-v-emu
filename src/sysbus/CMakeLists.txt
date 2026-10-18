add_library(sysbus STATIC sysbus.c)
target_link_libraries(sysbus PRIVATE helper)
target_include_directories(sysbus PRIVATE ${CMAKE_SOURCE_DIR}/src/helper
                                          ${CMAKE_SOURCE_DIR}/src/io_devs)

target_include_directories(sysbus PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
