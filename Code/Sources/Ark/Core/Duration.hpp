#pragma once

#include "Ark/Core/Types.hpp"
#include "Ark/Math/Intrinsics.hpp"

namespace Ark
{
    /// Represents a span of time measured in ticks.
    struct Duration final
    {
    private:
        static constexpr int64 ticksPerMillisecond = 10000;
        static constexpr int64 ticksPerMinute = 600000000;
        static constexpr int64 ticksPerSecond = 10000000;

        int64 ticks{0};

    public:
        /// Constructs a zero duration.
        Duration() = default;

        /// Constructs a duration from raw tick count.
        /// @param ticks The number of ticks.
        Duration(int64 ticks)
            : ticks{ticks}
        {
        }

        /// Creates a duration from milliseconds.
        /// @param milliseconds The duration in milliseconds.
        /// @return The resulting duration.
        static inline Duration fromMilliseconds(float64 milliseconds)
        {
            return {static_cast<int64>(milliseconds * ticksPerMillisecond)};
        }

        /// Creates a duration from minutes.
        /// @param minutes The duration in minutes.
        /// @return The resulting duration.
        static inline Duration fromMinutes(float64 minutes)
        {
            return {static_cast<int64>(minutes * ticksPerMinute)};
        }

        /// Creates a duration from seconds.
        /// @param seconds The duration in seconds.
        /// @return The resulting duration.
        static inline Duration fromSeconds(float64 seconds)
        {
            return {static_cast<int64>(seconds * ticksPerSecond)};
        }

        /// Returns the absolute value of the duration.
        /// @return A non-negative duration.
        inline Duration getAbsolute() const
        {
            return {Math::abs(ticks)};
        }

        /// Returns the millisecond component.
        /// @return Milliseconds in the range [0, 999].
        inline int32 getMilliseconds() const
        {
            return static_cast<int32>(ticks / ticksPerMillisecond % 1000);
        }

        /// Returns the minute component.
        /// @return Minutes in the range [0, 59].
        inline int32 getMinutes() const
        {
            return static_cast<int32>(ticks / ticksPerMinute % 60);
        }

        /// Returns the second component.
        /// @return Seconds in the range [0, 59].
        inline int32 getSeconds() const
        {
            return static_cast<int32>(ticks / ticksPerSecond % 60);
        }

        /// Returns the total duration in milliseconds.
        /// @return Total milliseconds.
        inline float64 getTotalMilliseconds() const
        {
            return static_cast<float64>(ticks) / ticksPerMillisecond;
        }

        /// Returns the total duration in minutes.
        /// @return Total minutes.
        inline float64 getTotalMinutes() const
        {
            return static_cast<float64>(ticks) / ticksPerMinute;
        }

        /// Returns the total duration in seconds.
        /// @return Total seconds.
        inline float64 getTotalSeconds() const
        {
            return static_cast<float64>(ticks) / ticksPerSecond;
        }

        inline bool operator==(Duration const& other) const
        {
            return (ticks == other.ticks);
        }

        inline bool operator!=(Duration const& other) const
        {
            return (ticks != other.ticks);
        }

        inline bool operator<(Duration const& other) const
        {
            return (ticks < other.ticks);
        }

        inline bool operator<=(Duration const& other) const
        {
            return (ticks <= other.ticks);
        }

        inline bool operator>(Duration const& other) const
        {
            return (ticks > other.ticks);
        }

        inline bool operator>=(Duration const& other) const
        {
            return (ticks >= other.ticks);
        }

        inline Duration operator-() const
        {
            return {-ticks};
        }

        inline Duration operator+(Duration const& other) const
        {
            return {ticks + other.ticks};
        }

        inline Duration& operator+=(Duration const& other)
        {
            ticks += other.ticks;
            return *this;
        }

        inline Duration operator-(Duration const& other) const
        {
            return {ticks - other.ticks};
        }

        inline Duration& operator-=(Duration const& other)
        {
            ticks -= other.ticks;
            return *this;
        }

        inline Duration operator*(float32 scalar) const
        {
            return {(int64)((float32)ticks * scalar)};
        }

        inline Duration& operator*=(float32 scalar)
        {
            ticks = static_cast<int64>(static_cast<float32>(ticks) * scalar);
            return *this;
        }
    };
}
