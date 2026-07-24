list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

include(CMake/Ark.cmake)
include(CMake/Options.cmake)
include(CMake/External.cmake)

ark_initialize()

add_subdirectory(Sources)

if(ARK_OPTION_BUILD_EXAMPLES)
    add_subdirectory(Examples)
endif()

if(ARK_OPTION_BUILD_TESTS)
    ark_build_tests()
endif()
