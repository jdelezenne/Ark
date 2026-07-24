#include "Ark/Application/Application.hpp"

namespace Ark::Application
{
    static EventDispatcher* globalEventDispatcher = nullptr;

    EventDispatcher* getEventDispatcher()
    {
        return globalEventDispatcher;
    }

    void setEventDispatcher(EventDispatcher* dispatcher)
    {
        globalEventDispatcher = dispatcher;
    }

    void initialize()
    {
        // Platform hooks that outlive a single EventLoop belong here.
        // Display reconfiguration / locale / theme observation is owned by EventSource today.
    }

    void finalize()
    {
        setEventDispatcher(nullptr);
    }
}
