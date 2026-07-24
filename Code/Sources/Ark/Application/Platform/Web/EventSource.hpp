#pragma once

#if defined(ARK_PLATFORM_WEBASSEMBLY)

#include "Ark/Application/EventSource.hpp"

namespace Ark::Platform::Web
{
    class WebEventSource final : public EventSource
    {
    public:
        void pollEvents(Collections::Array<Event>& outEvents) override;
        void waitEvents(Collections::Array<Event>& outEvents, uint32 timeoutMs) override;
    };
}

#endif
