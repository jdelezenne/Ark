#include "Ark/Application/EventDispatcher.hpp"

#include "Ark/Concurrency/Locks.hpp"

namespace Ark
{
    EventDispatcher::EventDispatcher() = default;
    EventDispatcher::~EventDispatcher() = default;

    void EventDispatcher::enqueueEvent(Event const& event)
    {
        Concurrency::LockGuard<Concurrency::Mutex> lock(mutex);
        eventQueue.append(event);
    }

    void EventDispatcher::enqueueEvent(Event&& event)
    {
        Concurrency::LockGuard<Concurrency::Mutex> lock(mutex);
        eventQueue.append(Ark::move(event));
    }

    void EventDispatcher::unsubscribe(int handlerId)
    {
        Concurrency::LockGuard<Concurrency::Mutex> lock(mutex);

        for (usize i = 0; i < handlers.getCount();)
        {
            if (handlers[i].id == handlerId)
            {
                handlers.removeAt(i);
            }
            else
            {
                ++i;
            }
        }
    }

    void EventDispatcher::dispatchPendingEvents()
    {
        Collections::Array<Event> events;
        Collections::Array<HandlerEntry> currentHandlers;

        {
            Concurrency::LockGuard<Concurrency::Mutex> lock(mutex);
            events = Ark::move(eventQueue);
            eventQueue.clear();
            currentHandlers = handlers;
        }

        for (auto const& event : events)
        {
            for (auto const& entry : currentHandlers)
            {
                entry.handler(event);
            }
        }
    }

    void EventDispatcher::clear()
    {
        Concurrency::LockGuard<Concurrency::Mutex> lock(mutex);
        eventQueue.clear();
    }
}
