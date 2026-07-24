#pragma once

#if defined(ARK_PLATFORM_WINDOWS)

#include "Ark/Application/EventSource.hpp"
#include "Ark/Collections/Array.hpp"
#include "Ark/Core/Types.hpp"

namespace Ark::Platform::Windows
{
    class WindowsEventSource final : public Ark::EventSource
    {
    public:
        WindowsEventSource();
        ~WindowsEventSource() override;

        void pollEvents(Collections::Array<Ark::Event>& outEvents) override;
        void waitEvents(Collections::Array<Ark::Event>& outEvents, uint32 timeoutMs) override;

    private:
        int64 lastClipboardSequence;
        bool isMonitoringTheme;
        bool isMonitoringClipboard;
    };
}

#endif
