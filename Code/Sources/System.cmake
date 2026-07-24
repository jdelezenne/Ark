ark_add_target(
    NAME Ark.System
    ALIAS Ark::System
    GROUP Ark/Modules
    TYPE STATIC
    DIRECTORY "Ark/System"
    LIBRARIES
        Ark::Core
        Ark::Collections
        Ark::Memory
        Ark::Strings
        Ark::Storage
)

if (ARK_PLATFORM_WINDOWS)
    target_sources(
        Ark.System
        PRIVATE
        Ark/System/Cursor.hpp
        Ark/System/Clipboard.hpp
        Ark/System/MessageBox.cpp
        Ark/System/MessageBox.hpp
        Ark/System/FileDialog.hpp
        Ark/System/Input.hpp
        Ark/System/Platform/Windows/Cursor.cpp
        Ark/System/Platform/Windows/Clipboard.cpp
        Ark/System/Platform/Windows/FileDialog.cpp
        Ark/System/Platform/Windows/MessageBox.cpp
        Ark/System/Platform/Windows/CpuInfo.cpp
        Ark/System/Platform/Windows/SystemInfo.cpp
        Ark/System/Platform/Windows/Environment.cpp
        Ark/System/Platform/Windows/KnownFolders.cpp
        Ark/System/Platform/Windows/Clock.cpp
        Ark/System/Platform/Windows/Console.cpp
        Ark/System/Platform/Windows/Process.cpp
        Ark/System/Platform/Windows/SharedLibrary.cpp
        Ark/System/Platform/Windows/Input.cpp
        Ark/System/Platform/Windows/Debug/Stacktrace.cpp
    )
elseif (ARK_PLATFORM_MACOS)
    target_sources(
        Ark.System
        PRIVATE
        Ark/System/Cursor.hpp
        Ark/System/Clipboard.hpp
        Ark/System/MessageBox.cpp
        Ark/System/MessageBox.hpp
        Ark/System/FileDialog.hpp
        Ark/System/Input.hpp
        Ark/System/Platform/MacOS/Cursor.mm
        Ark/System/Platform/MacOS/Clipboard.mm
        Ark/System/Platform/MacOS/FileDialog.mm
        Ark/System/Platform/MacOS/MessageBox.mm
        Ark/System/Platform/MacOS/CpuInfo.cpp
        Ark/System/Platform/MacOS/SystemInfo.cpp
        Ark/System/Platform/MacOS/KnownFolders.mm
        Ark/System/Platform/Posix/Environment.cpp
        Ark/System/Platform/MacOS/Clock.cpp
        Ark/System/Time.cpp
        Ark/System/Platform/Posix/Console.cpp
        Ark/System/Platform/Posix/Process.cpp
        Ark/System/Platform/MacOS/SharedLibrary.cpp
        Ark/System/Platform/MacOS/Input.mm
        Ark/System/Platform/Posix/Debug/Stacktrace.cpp
    )

    target_link_libraries(
        Ark.System
        PUBLIC
        "-framework Foundation"
        "-framework CoreFoundation"
        "-framework AppKit"
        "-framework UniformTypeIdentifiers"
        "-framework IOKit"
    )
elseif(ARK_PLATFORM_WEBASSEMBLY)
    target_sources(
        Ark.System
        PRIVATE
        Ark/System/Cursor.hpp
        Ark/System/Clipboard.hpp
        Ark/System/MessageBox.cpp
        Ark/System/MessageBox.hpp
        Ark/System/FileDialog.hpp
        Ark/System/Input.hpp
        Ark/System/Platform/Web/Cursor.cpp
        Ark/System/Platform/Web/Clipboard.cpp
        Ark/System/Platform/Web/MessageBox.cpp
        Ark/System/Platform/Web/FileDialog.cpp
        Ark/System/Platform/Web/Input.cpp
        Ark/System/Platform/Posix/Environment.cpp
        Ark/System/Platform/Posix/Clock.cpp
        Ark/System/Time.cpp
        Ark/System/Platform/Posix/Console.cpp
        Ark/System/Platform/Web/Debug/Stacktrace.cpp
    )
elseif(ARK_PLATFORM_LINUX)
    target_sources(
        Ark.System
        PRIVATE
        Ark/System/Cursor.hpp
        Ark/System/Clipboard.hpp
        Ark/System/MessageBox.cpp
        Ark/System/MessageBox.hpp
        Ark/System/FileDialog.hpp
        Ark/System/Input.hpp
        Ark/System/Platform/Linux/Cursor.cpp
        Ark/System/Platform/Linux/Clipboard.cpp
        Ark/System/Platform/Linux/FileDialog.cpp
        Ark/System/Platform/Linux/MessageBox.cpp
        Ark/System/Platform/Linux/CpuInfo.cpp
        Ark/System/Platform/Linux/SystemInfo.cpp
        Ark/System/Platform/Posix/Environment.cpp
        Ark/System/Platform/Linux/KnownFolders.cpp
        Ark/System/Platform/Posix/Clock.cpp
        Ark/System/Time.cpp
        Ark/System/Platform/Posix/Console.cpp
        Ark/System/Platform/Posix/Process.cpp
        Ark/System/Platform/Linux/SharedLibrary.cpp
        Ark/System/Platform/Linux/Input.cpp
        Ark/System/Platform/Posix/Debug/Stacktrace.cpp
    )

    find_package(X11 REQUIRED)
    target_link_libraries(
        Ark.System
        PUBLIC
        X11::X11
    )
else()
    target_sources(
        Ark.System
        PRIVATE
        Ark/System/Platform/Posix/Environment.cpp
        Ark/System/Platform/Posix/Clock.cpp
        Ark/System/Time.cpp
        Ark/System/Platform/Posix/Console.cpp
        Ark/System/Platform/Posix/Debug/Stacktrace.cpp
    )
endif()
