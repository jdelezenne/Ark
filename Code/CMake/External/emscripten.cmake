# If not configuring with the Emscripten toolchain, expose a hint variable and exit.
if(NOT CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    set(EMSCRIPTEN_TOOLCHAIN_FILE "" CACHE FILEPATH "Path to Emscripten toolchain file (use in presets)")
    if(EMSCRIPTEN_TOOLCHAIN_FILE AND NOT EXISTS "${EMSCRIPTEN_TOOLCHAIN_FILE}")
        message(FATAL_ERROR "EMSCRIPTEN_TOOLCHAIN_FILE is set but does not exist: ${EMSCRIPTEN_TOOLCHAIN_FILE}")
    endif()
    return()
endif()

# We are under the Emscripten toolchain. Validate toolchain file presence for clarity.
if(NOT CMAKE_TOOLCHAIN_FILE)
    message(WARNING "CMAKE_SYSTEM_NAME is Emscripten but CMAKE_TOOLCHAIN_FILE is not set. Ensure you configured with a toolchain file or 'emcmake cmake'.")
endif()

# Reasonable defaults for web builds; can be overridden per-target.
set(CMAKE_EXECUTABLE_SUFFIX ".html" CACHE INTERNAL "")
add_link_options(
    -sALLOW_MEMORY_GROWTH=1
)
