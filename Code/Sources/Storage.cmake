ark_add_target(
    NAME Ark.Storage
    ALIAS Ark::Storage
    GROUP Ark/Modules
    TYPE STATIC
    DIRECTORY "Ark/Storage"
    LIBRARIES
        Ark::Core
        Ark::Collections
        Ark::Memory
        Ark::Strings
)

if (ARK_PLATFORM_WINDOWS)
    target_sources(
        Ark.Storage
        PRIVATE
            Ark/Storage/Platform/Windows/File.cpp
            Ark/Storage/Platform/Windows/FileSystem.cpp
    )
elseif (ARK_PLATFORM_MACOS)
    target_sources(
        Ark.Storage
        PRIVATE
            Ark/Storage/Platform/Posix/File.cpp
            Ark/Storage/Platform/MacOS/FileSystem.cpp
    )
elseif (ARK_PLATFORM_WEBASSEMBLY OR ARK_PLATFORM_LINUX)
    target_sources(
        Ark.Storage
        PRIVATE
            Ark/Storage/Platform/Posix/File.cpp
            Ark/Storage/Platform/Posix/FileSystem.cpp
    )
else()
    target_sources(
        Ark.Storage
        PRIVATE
            Ark/Storage/Platform/CppStd/File.cpp
            Ark/Storage/Platform/CppStd/FileSystem.cpp
    )
endif()
