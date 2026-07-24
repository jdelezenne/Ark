#pragma once

#include "Ark/Application/Event.hpp"
#include "Ark/Collections/Array.hpp"
#include "Ark/Concurrency/Locks.hpp"
#include "Ark/Concurrency/Mutex.hpp"
#include "Ark/Core/Function.hpp"

namespace Ark
{
    /// Callback type for untyped event handlers.
    /// @param Event received from the dispatcher.
    using EventHandler = Function<void(Event const&)>;

    /// Callback type for typed event handlers.
    /// @tparam T Specific event type to handle.
    template <typename T>
    using TypedEventHandler = Function<void(T const&)>;

    /// Central event dispatcher for subscriptions and dispatch.
    /// Thread-safe event queue with handler subscription pattern.
    class EventDispatcher final
    {
    public:
        /// Constructs an event dispatcher.
        EventDispatcher();

        /// Destroys the dispatcher and clears queued events.
        ~EventDispatcher();

        /// Enqueues an event for later dispatch.
        /// @param event Event to enqueue (copied).
        void enqueueEvent(Event const& event);

        /// Enqueues an event for later dispatch (move).
        /// @param event Event to enqueue (moved).
        void enqueueEvent(Event&& event);

        /// Subscribes a handler to a specific event type.
        /// @tparam T Event type to listen for.
        /// @param handler Callback invoked when T events are dispatched.
        /// @return Handler subscription ID (used to unsubscribe).
        template <typename T>
        int subscribe(TypedEventHandler<T> handler)
        {
            Concurrency::LockGuard<Concurrency::Mutex> lock(mutex);

            int id = nextHandlerId++;

            handlers.append(HandlerEntry{
                .id = id,
                .handler = [h = Ark::move(handler)](Event const& event)
                {
                    if (T const* typedEvent = Ark::getIf<T>(&event))
                    {
                        h(*typedEvent);
                    }
                },
            });

            return id;
        }

        /// Unsubscribes a handler by ID.
        /// @param handlerId Subscription ID returned by subscribe().
        void unsubscribe(int handlerId);

        /// Dispatches all queued events to subscribed handlers.
        void dispatchPendingEvents();

        /// Clears all queued events and handlers.
        void clear();

    private:
        struct HandlerEntry final
        {
            int id = 0;
            EventHandler handler;
        };

    private:
        Concurrency::Mutex mutex;
        Collections::Array<Event> eventQueue;
        Collections::Array<HandlerEntry> handlers;
        int nextHandlerId = 1;
    };
}
