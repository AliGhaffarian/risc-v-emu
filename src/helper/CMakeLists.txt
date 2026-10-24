add_library(helper helper.c logger.c linkedlist.c)
target_include_directories(helper PRIVATE ${CMAKE_SOURCE_DIR}/src/cpu ${CMAKE_SOURCE_DIR}/src/sysbus)
target_include_directories(helper PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
