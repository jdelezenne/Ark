cmake_minimum_required(VERSION 4.2)

project(
    Ark
    VERSION 0.1.0
    LANGUAGES C CXX
)

include(CTest)

if(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    set(ARK_OPTION_BUILD_TESTS OFF CACHE BOOL "Disable tests on WebAssembly" FORCE)
endif()

set(ARK_ROOT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
set(ARK_BINARIES_ROOT_DIRECTORY "${CMAKE_SOURCE_DIR}/Binaries/Ark")
set(ARK_BINARIES_DIRECTORY "${ARK_BINARIES_ROOT_DIRECTORY}/${CMAKE_SYSTEM_NAME}/$<CONFIG>")

add_subdirectory(Code)
