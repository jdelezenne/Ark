#pragma once

#if defined(ARK_PLATFORM_LINUX) && !defined(ARK_PLATFORM_WEBASSEMBLY)

#include "Ark/Application/EventSource.hpp"

namespace Ark::Platform::Linux
{
    class LinuxEventSource final : public EventSource
    {
    public:
        void pollEvents(Collections::Array<Event>& outEvents) override;
        void waitEvents(Collections::Array<Event>& outEvents, uint32 timeoutMs) override;
    };
}

#endif
