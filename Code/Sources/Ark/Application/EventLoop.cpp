#include "Ark/Application/EventLoop.hpp"
#include "Ark/Application/Application.hpp"

namespace Ark
{
    EventLoop::EventLoop()
        : eventSource(EventSource::createPlatformSource())
    {
        Application::setEventDispatcher(&dispatcher);
    }

    EventLoop::~EventLoop()
    {
        if (Application::getEventDispatcher() == &dispatcher)
        {
            Application::setEventDispatcher(nullptr);
        }
    }

    void EventLoop::pollEvents()
    {
        if (eventSource == nullptr)
        {
            return;
        }

        platformEvents.clear();
        eventSource->pollEvents(platformEvents);

        for (auto const& event : platformEvents)
        {
            dispatcher.enqueueEvent(event);
        }

        dispatcher.dispatchPendingEvents();
    }

    void EventLoop::waitEvents(uint32 timeoutMs)
    {
        if (eventSource == nullptr)
        {
            return;
        }

        platformEvents.clear();
        eventSource->waitEvents(platformEvents, timeoutMs);

        for (auto const& event : platformEvents)
        {
            dispatcher.enqueueEvent(event);
        }

        dispatcher.dispatchPendingEvents();
    }

    EventDispatcher& EventLoop::getDispatcher()
    {
        return dispatcher;
    }

    EventDispatcher const& EventLoop::getDispatcher() const
    {
        return dispatcher;
    }
}
