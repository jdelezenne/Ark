#pragma once

#include "Ark/Core/Types.hpp"

namespace Ark
{
    using Ticks = int64;

    namespace TimeHelper
    {
        constexpr float64 millisecondsPerSecond = 1000.0;
        constexpr float64 microsecondsPerSecond = 1000000.0;
        constexpr float64 nanosecondsPerSecond = 1000000000.0;
        constexpr float64 nanosecondsPerMillisecond = 1000000.0;
        constexpr float64 nanosecondsPerMicrosecond = 1000.0;

        /// Converts seconds to milliseconds.
        /// @param seconds The time in seconds to convert.
        /// @return The time in milliseconds.
        constexpr float64 secondsToMilliseconds(float64 seconds)
        {
            return seconds * millisecondsPerSecond;
        }

        /// Converts milliseconds to seconds.
        /// @param milliseconds The time in milliseconds to convert.
        /// @return The time in seconds.
        constexpr float64 millisecondsToSeconds(float64 milliseconds)
        {
            return milliseconds / millisecondsPerSecond;
        }

        /// Converts milliseconds to nanoseconds.
        /// @param milliseconds The time in milliseconds to convert.
        /// @return The time in nanoseconds.
        constexpr float64 millisecondsToNanoseconds(float64 milliseconds)
        {
            return milliseconds * nanosecondsPerMillisecond;
        }

        /// Converts nanoseconds to milliseconds.
        /// @param nanoseconds The time in nanoseconds to convert.
        /// @return The time in milliseconds.
        constexpr float64 nanosecondsToMilliseconds(float64 nanoseconds)
        {
            return nanoseconds / nanosecondsPerMillisecond;
        }

        /// Converts microseconds to nanoseconds.
        /// @param microseconds The time in microseconds to convert.
        /// @return The time in nanoseconds.
        constexpr float64 microsecondsToNanoseconds(float64 microseconds)
        {
            return microseconds * nanosecondsPerMicrosecond;
        }

        /// Converts nanoseconds to microseconds.
        /// @param nanoseconds The time in nanoseconds to convert.
        /// @return The time in microseconds.
        constexpr float64 nanosecondsToMicroseconds(float64 nanoseconds)
        {
            return nanoseconds / nanosecondsPerMicrosecond;
        }
    }
}
