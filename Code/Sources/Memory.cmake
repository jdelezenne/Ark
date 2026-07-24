ark_add_target(
    NAME Ark.Memory
    ALIAS Ark::Memory
    GROUP Ark/Modules
    TYPE STATIC
    DIRECTORY "Ark/Memory"
    LIBRARIES
        Ark::Core
)

if (ARK_PLATFORM_WINDOWS)
    target_sources(
        Ark.Memory
        PRIVATE
        Ark/Memory/Platform/Windows/VirtualMemory.cpp
        Ark/Memory/Platform/Windows/StandardAllocator.cpp
    )
elseif (ARK_PLATFORM_MACOS)
    target_sources(
        Ark.Memory
        PRIVATE
        Ark/Memory/Platform/Posix/VirtualMemory.cpp
        Ark/Memory/Platform/MacOS/StandardAllocator.cpp
    )
elseif (ARK_PLATFORM_LINUX)
    target_sources(
        Ark.Memory
        PRIVATE
        Ark/Memory/Platform/Posix/VirtualMemory.cpp
        Ark/Memory/Platform/Generic/StandardAllocator.cpp
    )
else()
    # Generic StandardAllocator is freestanding and relies on VirtualMemory.
    # Unknown hosts must supply a VirtualMemory backend separately.
    target_sources(
        Ark.Memory
        PRIVATE
        Ark/Memory/Platform/Generic/StandardAllocator.cpp
    )
endif()
