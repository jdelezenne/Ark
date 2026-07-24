# Log a message
macro(ark_message level text)
    if(level STREQUAL "FATAL_ERROR")
        message(FATAL_ERROR "[Ark] ${text}")
    elseif(level STREQUAL "WARNING")
        message(WARNING "[Ark] ${text}")
    elseif(level STREQUAL "STATUS")
        message(STATUS "[Ark] ${text}")
    else()
        message("[Ark] ${text}")
    endif()
endmacro()

# Detect the platform
macro(ark_detect_platform)
    if(${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten")
        set(ARK_PLATFORM_LINUX ON)
        set(ARK_PLATFORM_WEBASSEMBLY ON)
        set(ARK_PLATFORM_NAME "WebAssembly")
    else()
        if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
            set(ARK_PLATFORM_WINDOWS ON)
            set(ARK_PLATFORM_NAME "Windows")

        elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
            set(ARK_PLATFORM_MACOS ON)
            set(ARK_PLATFORM_NAME "macOS")

        elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
            set(ARK_PLATFORM_LINUX ON)
            set(ARK_PLATFORM_NAME "Linux")
        endif()

        if(ARK_OPTION_PLATFORM_GDK AND(NOT PLATFORM_WINDOWS AND NOT PLATFORM_XBOX_SERIES))
            message(FATAL_ERROR "Can only enable GDK on Windows or Xbox Series.")
        endif()

        if(ARK_OPTION_PLATFORM_UWP AND(NOT PLATFORM_WINDOWS AND NOT PLATFORM_XBOX_ONE))
            message(FATAL_ERROR "Can only enable UWP on Windows or Xbox One.")
        endif()

        if(ARK_PLATFORM_WINDOWS)
            if(ARK_OPTION_PLATFORM_GDK)
                set(ARK_PLATFORM_WINDOWS_GDK ON)
                set(ARK_PLATFORM_NAME "WindowsGDK")
            elseif(ARK_OPTION_PLATFORM_UWP)
                set(ARK_PLATFORM_WINDOWS_UWP ON)
                set(ARK_PLATFORM_NAME "WindowsStore")
            else()
                set(ARK_PLATFORM_WINDOWS_DESKTOP ON)
                set(ARK_PLATFORM_NAME "WindowsDesktop")
            endif()
        endif()
    endif()

    ark_message(STATUS "Detected platform: ${ARK_PLATFORM_NAME}.")
endmacro()

# Detect the architecture
macro(ark_detect_architecture)
    if(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            set(ARK_ARCHITECTURE_X64 ON)
            set(ARK_ARCHITECTURE "WASM64")
        else()
            set(ARK_ARCHITECTURE_X86 ON)
            set(ARK_ARCHITECTURE "WASM32")
        endif()

    elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
        set(ARK_ARCHITECTURE_X64 ON)
        set(ARK_ARCHITECTURE "X64")

    elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "i686" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "i386")
        set(ARK_ARCHITECTURE_X86 ON)
        set(ARK_ARCHITECTURE "X86")

    elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "arm64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
        set(ARK_ARCHITECTURE_ARM64 ON)
        set(ARK_ARCHITECTURE "ARM64")

    else()
        ark_message(FATAL_ERROR "Unsupported architecture.")
    endif()

    ark_message(STATUS "Detected architecture: ${ARK_ARCHITECTURE}.")
endmacro()

macro(ark_initialize)
    include(FetchContent)
    include(GNUInstallDirs)

    # Set the C++ standard and disable extensions for portability
    set(CMAKE_CXX_STANDARD 23)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)

    # Disable automatic module dependency scanning (MSVC defaults to ON for C++20+)
    # Prevents corrupted .module.json files when not using C++ modules
    if(MSVC)
        set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
    endif()

    # Set minimum macOS deployment target for modern APIs
    if(APPLE)
        set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13" CACHE STRING "Minimum macOS deployment version")
    endif()

    # Suppress the ZERO_CHECK target
    set(CMAKE_SUPPRESS_REGENERATION ON)

    # Organize targets into folders in generated project files
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)

    # Enable CMake policies to deduplicate libraries on link lines
    # This prevents duplicate library warnings from the linker
    if(POLICY CMP0156)
        cmake_policy(SET CMP0156 NEW)
    endif()
    if(POLICY CMP0179)
        cmake_policy(SET CMP0179 NEW)
    endif()

    set(ARK_GENERATED_INCLUDE_DIRECTORY "${CMAKE_BINARY_DIR}/generated")
    file(MAKE_DIRECTORY "${ARK_GENERATED_INCLUDE_DIRECTORY}/Ark/Core")
    configure_file(
        "${CMAKE_SOURCE_DIR}/Code/CMake/ArkVersion.hpp.in"
        "${ARK_GENERATED_INCLUDE_DIRECTORY}/Ark/Core/ArkVersion.hpp"
        @ONLY
    )

    ark_detect_platform()
    ark_detect_architecture()
endmacro()

macro(ark_build_tests)
    include(CTest)

    if(NOT TARGET Catch2::Catch2)
        include(External/catch2)
    endif()

    if(DEFINED Catch2_SOURCE_DIR AND EXISTS "${Catch2_SOURCE_DIR}/extras/Catch.cmake")
        list(APPEND CMAKE_MODULE_PATH "${Catch2_SOURCE_DIR}/extras")
    endif()
    include(Catch)

    if(DEFINED ARK_TESTS_SOURCE_DIR)
        add_subdirectory(${ARK_TESTS_SOURCE_DIR})
    elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Tests/CMakeLists.txt")
        add_subdirectory(Tests)
    endif()
endmacro(ark_build_tests)

function(ark_add_target)
    set(options)
    set(oneValueArgs NAME TYPE ALIAS GROUP DIRECTORY INCLUDE)
    set(multiValueArgs SOURCES LIBRARIES)

    cmake_parse_arguments(TARGET "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT TARGET_NAME)
        message(FATAL_ERROR "[Ark] ark_add_target: NAME is required")
    endif()

    if(NOT TARGET_TYPE)
        message(FATAL_ERROR "[Ark] ark_add_target: TYPE is required")
    endif()

    if(NOT TARGET_DIRECTORY AND NOT TARGET_SOURCES)
        message(FATAL_ERROR "[Ark] ark_add_target: DIRECTORY or SOURCES is required")
    endif()

    if(NOT TARGET_SOURCES)
        set(source_dir "${CMAKE_CURRENT_SOURCE_DIR}/${TARGET_DIRECTORY}")
    
        file(GLOB_RECURSE source_files CONFIGURE_DEPENDS
            "${source_dir}/*.cpp"
            "${source_dir}/*.c"
            "${source_dir}/*.mm"
            "${source_dir}/*.m"
        )

        foreach(source_file IN LISTS source_files)
            if(source_file MATCHES "/Platform/")
                list(REMOVE_ITEM source_files ${source_file})
            endif()
        endforeach()

        list(APPEND TARGET_SOURCES ${source_files})

        if(NOT TARGET_SOURCES)
            if(NOT TARGET_TYPE STREQUAL "INTERFACE")
                message(FATAL_ERROR "[Ark] No source files found for target: ${TARGET_NAME}")
            endif()
        endif()
    endif()

    if(TARGET_DIRECTORY AND TARGET_SOURCES)
        set(full_path_sources "")
        foreach(source_file IN LISTS TARGET_SOURCES)
            if(IS_ABSOLUTE "${source_file}")
                list(APPEND full_path_sources "${source_file}")
            else()
                list(APPEND full_path_sources "${CMAKE_CURRENT_SOURCE_DIR}/${TARGET_DIRECTORY}/${source_file}")
            endif()
        endforeach()
        set(TARGET_SOURCES ${full_path_sources})
    endif()

    message(STATUS "[Ark] Creating target: ${TARGET_NAME}")
    message(STATUS "[Ark] Type: ${TARGET_TYPE}")
    message(DEBUG "[Ark] Sources: ${TARGET_SOURCES}")
    message(STATUS "[Ark] Libraries: ${TARGET_LIBRARIES}")

    if(TARGET_TYPE STREQUAL "EXECUTABLE" OR TARGET_TYPE STREQUAL "CONSOLE")
        if(ARK_PLATFORM_WINDOWS AND TARGET_TYPE STREQUAL "EXECUTABLE")
            add_executable(
                ${TARGET_NAME}
                WIN32
                ${TARGET_SOURCES}
            )
        else()
            add_executable(
                ${TARGET_NAME}
                ${TARGET_SOURCES}
            )
        endif()

    elseif(TARGET_TYPE STREQUAL "STATIC" OR TARGET_TYPE STREQUAL "SHARED")
        add_library(
            ${TARGET_NAME}
            ${TARGET_TYPE}
            ${TARGET_SOURCES}
        )
    elseif(TARGET_TYPE STREQUAL "INTERFACE")
        add_library(
            ${TARGET_NAME}
            INTERFACE
        )
    endif()

    # Create an alias for the target
    if(TARGET_ALIAS)
        add_library(
            ${TARGET_ALIAS}
            ALIAS
            ${TARGET_NAME}
        )
    endif()

    if(TARGET_NAME MATCHES "^Ark\\.(.+)$")
        set_target_properties(
            ${TARGET_NAME}
            PROPERTIES
                EXPORT_NAME "${CMAKE_MATCH_1}"
        )
    endif()

    if(TARGET_TYPE STREQUAL "EXECUTABLE" OR TARGET_TYPE STREQUAL "CONSOLE")
        if(ARK_PLATFORM_WINDOWS)
            if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Platform/Windows")
                target_sources(
                    ${TARGET_NAME}
                    PRIVATE
                    "${CMAKE_CURRENT_SOURCE_DIR}/Platform/Windows/App.rc"
                )
            elseif(EXISTS "${CMAKE_SOURCE_DIR}/Code/Platform/Windows/App.rc")
                target_sources(
                    ${TARGET_NAME}
                    PRIVATE
                    "${CMAKE_SOURCE_DIR}/Code/Platform/Windows/App.rc"
                )
            endif()

            target_compile_definitions(
                ${TARGET_NAME}
                PUBLIC
                ARK_PLATFORM_WINDOWS_SUBSYSTEM
            )
        endif()
    endif()

    if(TARGET_TYPE STREQUAL "INTERFACE")
        target_link_libraries(
            ${TARGET_NAME}
            INTERFACE
            ${TARGET_LIBRARIES}
        )
    else()
        target_link_libraries(
            ${TARGET_NAME}
            PUBLIC
            ${TARGET_LIBRARIES}
        )
    endif()

    if(TARGET_INCLUDE)
        set(INCLUDE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${TARGET_INCLUDE}")
    else()
        set(INCLUDE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
    endif()

    if(TARGET_TYPE STREQUAL "INTERFACE")
        target_include_directories(
            ${TARGET_NAME}
            INTERFACE
            $<BUILD_INTERFACE:${INCLUDE_DIRECTORY}>
            $<BUILD_INTERFACE:${ARK_GENERATED_INCLUDE_DIRECTORY}>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        )
    else()
        target_include_directories(
            ${TARGET_NAME}
            PUBLIC
            $<BUILD_INTERFACE:${INCLUDE_DIRECTORY}>
            $<BUILD_INTERFACE:${ARK_GENERATED_INCLUDE_DIRECTORY}>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        )
    endif()

    if(NOT TARGET_TYPE STREQUAL "INTERFACE")
        set_target_properties(
            ${TARGET_NAME} PROPERTIES
            CXX_STANDARD 23
            CXX_STANDARD_REQUIRED ON
            CXX_EXTENSIONS OFF
        )
    endif()

    # Ensure Objective-C++ (macOS) targets use the same modern C++ standard
    if(APPLE AND NOT TARGET_TYPE STREQUAL "INTERFACE")
        set_target_properties(
            ${TARGET_NAME}
            PROPERTIES
            OBJCXX_STANDARD 23
            OBJCXX_STANDARD_REQUIRED ON
            OBJCXX_EXTENSIONS OFF
        )
    endif()

    set(_ark_scope PUBLIC)
    if(TARGET_TYPE STREQUAL "INTERFACE")
        set(_ark_scope INTERFACE)
    endif()

    # Disable exceptions / RTTI
    if(CLANG)
        target_compile_options(
            ${TARGET_NAME}
            ${_ark_scope}
            -fno-exceptions
            -Wno-invalid-token-paste
            -Wno-c99-designator
            -Wno-reorder-init-list
        )
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(
            ${TARGET_NAME}
            ${_ark_scope}
            -fno-exceptions
            -Wno-c99-designator
            -Wno-reorder-init-list
        )
    elseif(MSVC)
        target_compile_options(
            ${TARGET_NAME}
            ${_ark_scope}
            "/EHsc"
        )
    endif()

    if(MSVC)
        target_compile_options(
            ${TARGET_NAME}
            ${_ark_scope}
            "/GR-"
        )
    endif()

    # Set platform-specific compile definitions
    if(ARK_PLATFORM_WINDOWS)
        target_compile_definitions(${TARGET_NAME} ${_ark_scope} ARK_PLATFORM_WINDOWS)

        if(ARK_PLATFORM_WINDOWS_UWP)
            target_compile_definitions(${TARGET_NAME} ${_ark_scope} ARK_PLATFORM_WINDOWS_UWP)
        elseif(ARK_PLATFORM_WINDOWS_GDK)
            target_compile_definitions(${TARGET_NAME} ${_ark_scope} ARK_PLATFORM_WINDOWS_GDK)
        endif()

    elseif(ARK_PLATFORM_MACOS)
        target_compile_definitions(${TARGET_NAME} ${_ark_scope} ARK_PLATFORM_MACOS)

    elseif(ARK_PLATFORM_LINUX)
        target_compile_definitions(${TARGET_NAME} ${_ark_scope} ARK_PLATFORM_LINUX)

        if(ARK_PLATFORM_WEBASSEMBLY)
            target_compile_definitions(${TARGET_NAME} ${_ark_scope} ARK_PLATFORM_WEBASSEMBLY)
        endif()
    endif()

    if(CLANG)
        target_compile_definitions(${TARGET_NAME} ${_ark_scope} ARK_COMPILER_CLANG)

    elseif(MSVC)
        target_compile_definitions(${TARGET_NAME} ${_ark_scope} ARK_COMPILER_MSVC)
    endif()

    if(ARK_OPTION_STANDARD_LIBRARY STREQUAL "cpp")
        target_compile_definitions(${TARGET_NAME} ${_ark_scope} ARK_USE_STANDARD_LIBRARY=ARK_STANDARD_LIBRARY_CPP)
    elseif(ARK_OPTION_STANDARD_LIBRARY STREQUAL "c")
        target_compile_definitions(${TARGET_NAME} ${_ark_scope} ARK_USE_STANDARD_LIBRARY=ARK_STANDARD_LIBRARY_C)
    elseif(ARK_OPTION_STANDARD_LIBRARY STREQUAL "generic")
        target_compile_definitions(${TARGET_NAME} ${_ark_scope} ARK_USE_STANDARD_LIBRARY=ARK_STANDARD_LIBRARY_GENERIC)
    endif()

    # Set the output directory (skip for INTERFACE)
    if(NOT TARGET_TYPE STREQUAL "INTERFACE")
        set(BINARY_DIR "${ARK_BINARIES_DIRECTORY}")
        set(TARGET_DIR "${TARGET_NAME}")

        set_target_properties(
            ${TARGET_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${BINARY_DIR}/${TARGET_DIR}"
            LIBRARY_OUTPUT_DIRECTORY "${BINARY_DIR}/${TARGET_DIR}"
            ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${TARGET_DIR}"
        )
    endif()

    if(TARGET_GROUP)
        set_target_properties(
            ${TARGET_NAME} PROPERTIES
            FOLDER ${TARGET_GROUP}
        )
    endif()
endfunction()
