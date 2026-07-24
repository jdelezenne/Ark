#include "Ark/Display/Window.hpp"

#if defined(ARK_PLATFORM_WINDOWS)
#include "Ark/Display/Platform/Windows/Window.hpp"
#elif defined(ARK_PLATFORM_MACOS)
#include "Ark/Display/Platform/MacOS/Window.hpp"
#elif defined(ARK_PLATFORM_WEBASSEMBLY)
#include "Ark/Display/Platform/Web/Window.hpp"
#elif defined(ARK_PLATFORM_LINUX)
#include "Ark/Display/Platform/Linux/Window.hpp"
#endif

namespace Ark::Display
{
    Window* Window::create(WindowCreateInfo const& info)
    {
#if defined(ARK_PLATFORM_WINDOWS)
        return Platform::Windows::createWindow(info);
#elif defined(ARK_PLATFORM_MACOS)
        return Platform::MacOS::createWindow(info);
#elif defined(ARK_PLATFORM_WEBASSEMBLY)
        return Platform::Web::createWindow(info);
#elif defined(ARK_PLATFORM_LINUX)
        return Platform::Linux::createWindow(info);
#else
        return nullptr;
#endif
    }
}
