#include "Ark/Application/EventSource.hpp"

#if defined(ARK_PLATFORM_MACOS)
#include "Ark/Application/Platform/MacOS/EventSource.hpp"
#elif defined(ARK_PLATFORM_WINDOWS)
#include "Ark/Application/Platform/Windows/EventSource.hpp"
#elif defined(ARK_PLATFORM_WEBASSEMBLY)
#include "Ark/Application/Platform/Web/EventSource.hpp"
#elif defined(ARK_PLATFORM_LINUX)
#include "Ark/Application/Platform/Linux/EventSource.hpp"
#endif

namespace Ark
{
    UniquePointer<EventSource> EventSource::createPlatformSource()
    {
#if defined(ARK_PLATFORM_MACOS)
        return makeUnique<Platform::MacOS::MacOSEventSource>();
#elif defined(ARK_PLATFORM_WINDOWS)
        return makeUnique<Platform::Windows::WindowsEventSource>();
#elif defined(ARK_PLATFORM_WEBASSEMBLY)
        return makeUnique<Platform::Web::WebEventSource>();
#elif defined(ARK_PLATFORM_LINUX)
        return makeUnique<Platform::Linux::LinuxEventSource>();
#else
        return nullptr;
#endif
    }
}
