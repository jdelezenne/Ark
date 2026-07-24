#pragma once

#include "Ark/Core/Function.hpp"
#include "Ark/Core/Types.hpp"

namespace Ark::System::Timer
{
    /// Identifier for a scheduled timer
    using TimerId = uint64;

    /// Callback invoked when a timer fires; return next interval in ms, or 0 to cancel
    using TimerCallback = Function<uint32(uint32)>;

    /// Schedule a timer callback after the given interval, repeating based on callback return value
    /// @param intervalMs Initial delay in milliseconds
    /// @param callback Function returning the next interval in milliseconds, or 0 to cancel
    /// @return A timer identifier; 0 indicates failure
    TimerId addTimer(uint32 intervalMs, TimerCallback callback);

    /// Remove a previously scheduled timer
    /// @param id The timer identifier returned by addTimer
    /// @return True if a timer was found and removed; false otherwise
    bool removeTimer(TimerId id);
}
