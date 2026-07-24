set(ARK_EXAMPLES
    Main
    Assert
    SystemConsole
    SystemInfo
    CpuInfo
    Logging
    LoggingCategories
    Timer
    MessageBox
    FileDialog
    Stacktrace
    Math
    Memory
    Storage
    Path
    VirtualFileSystem
    IniDocument
    Display
    Window
    Events
    Clipboard
    Noise
    Cube
)

foreach(EXAMPLE ${ARK_EXAMPLES})
    set(target "ArkExample${EXAMPLE}")
    set(example_dir "${CMAKE_CURRENT_SOURCE_DIR}/${EXAMPLE}")

    unset(ARK_EXAMPLE_LIBRARIES)
    if(EXISTS "${example_dir}/${EXAMPLE}.cmake")
        include("${EXAMPLE}/${EXAMPLE}.cmake")
    endif()

    # Fallback to default libraries if none provided by the example
    if(NOT ARK_EXAMPLE_LIBRARIES)
        set(ARK_EXAMPLE_LIBRARIES
            Ark::Core
            Ark::Memory
            Ark::System
        )
    endif()

    ark_add_target(
        NAME ${target}
        GROUP Ark/Examples
        TYPE EXECUTABLE
        DIRECTORY ${EXAMPLE}
        SOURCES
        LIBRARIES ${ARK_EXAMPLE_LIBRARIES}
    )

    # Place all example executables into a shared Examples directory
    set_target_properties(
        ${target}
        PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${ARK_BINARIES_DIRECTORY}/Examples"
            LIBRARY_OUTPUT_DIRECTORY "${ARK_BINARIES_DIRECTORY}/Examples"
            ARCHIVE_OUTPUT_DIRECTORY "${ARK_BINARIES_DIRECTORY}/Examples"
    )
endforeach()
