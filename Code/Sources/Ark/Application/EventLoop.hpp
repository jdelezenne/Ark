#pragma once

#include "Ark/Application/EventDispatcher.hpp"
#include "Ark/Application/EventSource.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Memory/UniquePointer.hpp"

namespace Ark
{
    /// Main event loop for processing system and user events.
    class EventLoop final
    {
    public:
        /// Constructs an event loop with a platform-specific event source.
        EventLoop();

        /// Destroys the event loop.
        ~EventLoop();

        EventLoop(EventLoop const&) = delete;
        EventLoop& operator=(EventLoop const&) = delete;

        /// Polls events without blocking.
        /// Returns immediately even if no events are available.
        void pollEvents();

        /// Waits for events with optional timeout.
        /// Blocks until an event arrives or timeout expires.
        /// @param timeoutMs Timeout in milliseconds (0 = no wait).
        void waitEvents(uint32 timeoutMs = 0);

        /// Returns the dispatcher for subscribing to events.
        EventDispatcher& getDispatcher();

        /// Returns the dispatcher (const).
        EventDispatcher const& getDispatcher() const;

    private:
        UniquePointer<EventSource> eventSource;
        EventDispatcher dispatcher;
        Collections::Array<Event> platformEvents;
    };
}
