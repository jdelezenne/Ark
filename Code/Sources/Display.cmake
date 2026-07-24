ark_add_target(
    NAME Ark.Display
    ALIAS Ark::Display
    GROUP Ark/Modules
    TYPE STATIC
    DIRECTORY "Ark/Display"
    LIBRARIES
        Ark::Core
        Ark::Collections
        Ark::Strings
        Ark::Math
        Ark::System
)

if (ARK_PLATFORM_WINDOWS)
    target_sources(
        Ark.Display
        PRIVATE
            Ark/Display/Platform/Windows/Monitor.cpp
            Ark/Display/Platform/Windows/Window.cpp
    )

    target_link_libraries(
        Ark.Display
        PUBLIC
            dxgi
            d3d11
    )
elseif (ARK_PLATFORM_MACOS)
    target_sources(
        Ark.Display
        PRIVATE
            Ark/Display/Platform/MacOS/Monitor.cpp
            Ark/Display/Platform/MacOS/Window.mm
    )

    target_link_libraries(
        Ark.Display
        PUBLIC
            "-framework AppKit"
            "-framework CoreGraphics"
            "-framework CoreFoundation"
    )
elseif (ARK_PLATFORM_WEBASSEMBLY)
    target_sources(
        Ark.Display
        PRIVATE
            Ark/Display/Platform/Web/Monitor.cpp
            Ark/Display/Platform/Web/Window.cpp
    )
elseif (ARK_PLATFORM_LINUX)
    target_sources(
        Ark.Display
        PRIVATE
            Ark/Display/Platform/Linux/X11Context.cpp
            Ark/Display/Platform/Linux/Monitor.cpp
            Ark/Display/Platform/Linux/Window.cpp
    )

    find_package(X11 REQUIRED)
    target_link_libraries(
        Ark.Display
        PUBLIC
            X11::X11
    )
    if (TARGET X11::Xrandr)
        target_link_libraries(Ark.Display PUBLIC X11::Xrandr)
    elseif (X11_Xrandr_FOUND)
        target_link_libraries(Ark.Display PUBLIC ${X11_Xrandr_LIB})
        target_include_directories(Ark.Display PRIVATE ${X11_Xrandr_INCLUDE_PATH})
    else()
        message(FATAL_ERROR "[Ark] Xrandr is required for Ark.Display on Linux")
    endif()
endif()
