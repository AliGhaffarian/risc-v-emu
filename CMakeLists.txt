cmake_minimum_required(VERSION 3.31)

find_program(CLANG_BIN clang REQUIRED)
set(CMAKE_C_COMPILER ${CLANG_BIN})

project("risc_v emulator" C)

include(FetchContent)
include(CTest)

set(CMAKE_C_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

option(EMU_BUILD_TESTING "enable testing" ${BUILD_TESTING})
option(EMU_BUILD_CHECKS "enable all checks and sanitizers")

add_subdirectory(src)

if(EMU_BUILD_CHECKS)
    set(
        CMAKE_C_CLANG_TIDY "clang-tidy")

    set(
        CMAKE_C_INCLUDE_WHAT_YOU_USE "iwyu;-Xiwyu;any;-Xiwyu;iwyu;-Xiwyu;args;")

    set(
        CMAKE_C_LINK_WHAT_YOU_USE)
    add_compile_options("-g")
endif()

if(EMU_BUILD_TESTING)
    add_subdirectory(tests)
endif()
