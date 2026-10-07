add_library(helper helper.c logger.c)
target_include_directories(helper PRIVATE ${CMAKE_SOURCE_DIR}/src/cpu)
