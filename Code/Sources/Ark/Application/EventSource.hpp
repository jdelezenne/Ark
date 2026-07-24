#pragma once

#include "Ark/Application/Event.hpp"
#include "Ark/Collections/Array.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Memory/UniquePointer.hpp"

namespace Ark
{
    /// Abstract event source interface.
    /// Platform-specific implementations pump events into the application.
    class EventSource
    {
    public:
        /// Destroys the event source.
        virtual ~EventSource() = default;

        /// Polls pending events without blocking.
        /// @param outEvents Output array to fill with available events.
        virtual void pollEvents(Collections::Array<Event>& outEvents) = 0;

        /// Waits for events with optional timeout.
        /// @param outEvents Output array to fill with events.
        /// @param timeoutMs Timeout in milliseconds.
        virtual void waitEvents(Collections::Array<Event>& outEvents, uint32 timeoutMs) = 0;

        /// Creates a platform-specific event source.
        /// @return Unique pointer to the platform event source.
        static UniquePointer<EventSource> createPlatformSource();
    };
}
