FetchContent_Declare(
    unity_download
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG        master
    GIT_SHALLOW    TRUE  # Adds --depth 1 to git clone
)
FetchContent_MakeAvailable(unity_download)

set_target_properties(unity PROPERTIES
    C_CLANG_TIDY ""
    C_INCLUDE_WHAT_YOU_USE ""
    C_LINK_WHAT_YOU_USE ""
)

get_target_property(UNITY_INCLUDES unity INTERFACE_INCLUDE_DIRECTORIES)
set_target_properties(unity PROPERTIES
    INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${UNITY_INCLUDES}")

add_subdirectory(cpu)
