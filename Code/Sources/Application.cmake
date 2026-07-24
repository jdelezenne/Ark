ark_add_target(
    NAME Ark.Application
    ALIAS Ark::Application
    GROUP Ark/Modules
    TYPE STATIC
    DIRECTORY "Ark/Application"
    LIBRARIES
        Ark::Core
        Ark::Collections
        Ark::Strings
        Ark::Memory
        Ark::Concurrency
        Ark::System
)

target_link_libraries(
    Ark.Display
    PRIVATE
        Ark::Application
)

if (ARK_PLATFORM_MACOS)
    target_sources(
        Ark.Application
        PRIVATE
            Ark/Application/Platform/MacOS/EventSource.mm
    )

    target_link_libraries(
        Ark.Application
        PUBLIC
            "-framework AppKit"
            "-framework CoreGraphics"
    )
elseif (ARK_PLATFORM_WINDOWS)
    target_sources(
        Ark.Application
        PRIVATE
            Ark/Application/Platform/Windows/EventSource.cpp
    )
elseif (ARK_PLATFORM_WEBASSEMBLY)
    target_sources(
        Ark.Application
        PRIVATE
            Ark/Application/Platform/Web/EventSource.cpp
    )
elseif (ARK_PLATFORM_LINUX)
    target_sources(
        Ark.Application
        PRIVATE
            Ark/Application/Platform/Linux/EventSource.cpp
    )

    target_link_libraries(
        Ark.Application
        PRIVATE
            Ark::Display
    )
endif()
