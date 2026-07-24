#pragma once

#include "Ark/Application/EventDispatcher.hpp"

namespace Ark
{
    class EventDispatcher;

    namespace Application
    {
        /// Event dispatcher instance for the application.
        /// @return Application-wide event dispatcher.
        EventDispatcher* getEventDispatcher();

        /// Sets the event dispatcher instance.
        /// @param dispatcher New dispatcher, or nullptr.
        void setEventDispatcher(EventDispatcher* dispatcher);

        /// Initializes platform-wide application hooks.
        /// Called once at startup to set up system integration.
        void initialize();

        /// Finalizes platform-wide application hooks.
        /// Called once at shutdown to clean up system resources.
        void finalize();
    }
}
