add_executable(cli cli.c)
target_link_libraries(cli PRIVATE cpu)
target_link_libraries(cli PRIVATE helper)
target_include_directories(cli PRIVATE ${CMAKE_SOURCE_DIR}/src/helper ${CMAKE_SOURCE_DIR}/src/cpu)
