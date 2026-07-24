#pragma once

#if defined(ARK_PLATFORM_MACOS)

#include "Ark/Application/EventSource.hpp"
#include "Ark/Collections/Array.hpp"
#include "Ark/Concurrency/Locks.hpp"
#include "Ark/Concurrency/Mutex.hpp"

namespace Ark::Platform::MacOS
{
    class MacOSEventSource final : public Ark::EventSource
    {
    public:
        MacOSEventSource();
        ~MacOSEventSource() override;

        void pollEvents(Collections::Array<Ark::Event>& outEvents) override;
        void waitEvents(Collections::Array<Ark::Event>& outEvents, uint32 timeoutMs) override;

        void enqueuePending(Ark::Event&& event);

    private:
        void drainPending(Collections::Array<Ark::Event>& outEvents);

        void* notificationObservers;
        int64 lastPasteboardChangeCount;
        Concurrency::Mutex pendingMutex;
        Collections::Array<Ark::Event> pendingEvents;
    };
}

#endif
