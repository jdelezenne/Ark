include(Core.cmake)
include(Collections.cmake)
include(Compression.cmake)
include(Concurrency.cmake)
include(Cryptography.cmake)
include(Documents.cmake)
include(Imaging.cmake)
include(Logging.cmake)
include(Math.cmake)
include(Memory.cmake)
include(Strings.cmake)
include(Storage.cmake)
include(System.cmake)
include(Display.cmake)
include(Application.cmake)

set(ARK_MODULE_TARGETS
    Ark.Core
    Ark.Collections
    Ark.Compression
    Ark.Concurrency
    Ark.Cryptography
    Ark.Documents
    Ark.Imaging
    Ark.Logging
    Ark.Math
    Ark.Memory
    Ark.Strings
    Ark.Storage
    Ark.System
    Ark.Display
    Ark.Application
)

include(CMakePackageConfigHelpers)

if(ARK_OPTION_EXTERNAL_DEPENDENCIES STREQUAL "system")
    install(
        TARGETS ${ARK_MODULE_TARGETS}
        EXPORT ArkTargets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
else()
    install(
        TARGETS ${ARK_MODULE_TARGETS}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
endif()

install(
    DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Ark"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    FILES_MATCHING PATTERN "*.hpp"
)

install(
    DIRECTORY "${ARK_GENERATED_INCLUDE_DIRECTORY}/Ark"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    FILES_MATCHING PATTERN "*.hpp"
)

if(ARK_OPTION_EXTERNAL_DEPENDENCIES STREQUAL "system")
    install(
        EXPORT ArkTargets
        NAMESPACE Ark::
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/Ark"
        FILE ArkTargets.cmake
    )

    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/ArkConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
    )

    configure_package_config_file(
        "${CMAKE_SOURCE_DIR}/Code/CMake/ArkConfig.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/ArkConfig.cmake"
        INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/Ark"
    )

    install(
        FILES
            "${CMAKE_CURRENT_BINARY_DIR}/ArkConfig.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/ArkConfigVersion.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/Ark"
    )
endif()
