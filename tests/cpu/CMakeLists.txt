include_directories(${CMAKE_SOURCE_DIR}/src/cpu)

add_executable(test_cpu cpu.c)
target_link_libraries(test_cpu unity)
target_link_libraries(test_cpu cpu)
add_test(NAME test_cpu COMMAND test_cpu)
