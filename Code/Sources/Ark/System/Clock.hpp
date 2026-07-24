#pragma once

#include "Ark/Core/Result.hpp"
#include "Ark/Core/Time.hpp"

namespace Ark::System
{
    /// The Clock class provides a way to measure time intervals and get the current time.
    struct Clock final
    {
        ARK_STATIC_STRUCT(Clock);

    public:
        /// Initializes the clock.
        static Outcome initialize();

        /// Gets the frequency of the clock in ticks per second.
        /// @return The frequency of the clock in ticks per second.
        static Ticks getFrequency();

        /// Gets the current time in ticks.
        static Ticks getTicks();

        /// Gets the current time in seconds.
        static float64 getTimeSeconds();

        /// Gets the current time in milliseconds.
        static float64 getTimeMilliseconds();
    };
}
