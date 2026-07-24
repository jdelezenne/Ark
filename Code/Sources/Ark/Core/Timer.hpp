#pragma once

#include "Ark/Core/Macros.hpp"
#include "Ark/Core/Time.hpp"
#include "Ark/System/Clock.hpp"

namespace Ark
{
    /// Lightweight timer used to measure elapsed ticks between stamps.
    class Timer final
    {
    private:
        Ticks previousTicks{};
        Ticks currentTicks{};

    public:
        /// Captures a new timestamp and keeps the previous one.
        inline void stamp()
        {
            previousTicks = currentTicks;
            currentTicks = System::Clock::getTicks();
        }

        /// Returns elapsed ticks between the last two stamps.
        /// @return Elapsed ticks.
        inline Ticks getElapsedTimeTicks() const
        {
            return currentTicks - previousTicks;
        }

        /// Returns elapsed time in seconds between the last two stamps.
        /// @return Elapsed seconds.
        inline float32 getElapsedTimeSeconds() const
        {
            Ticks const frequency = System::Clock::getFrequency();
            if (frequency == 0)
            {
                return 0.0f;
            }

            return static_cast<float32>(getElapsedTimeTicks()) / static_cast<float32>(frequency);
        }
    };

    /// Scoped helper that writes measured elapsed ticks on destruction.
    struct ScopedTimer final
    {
    private:
        Ticks& ticks;
        Timer timer;

    public:
        ARK_NOT_COPYABLE(ScopedTimer)
        ARK_NOT_MOVABLE(ScopedTimer)

        /// Starts timing and stores the destination reference.
        /// @param ticks Destination where elapsed ticks are written on destruction.
        explicit ScopedTimer(Ticks& ticks)
            : ticks{ticks}
        {
            timer.stamp();
        }

        /// Stops timing and stores elapsed ticks into the destination.
        ~ScopedTimer()
        {
            timer.stamp();
            ticks = timer.getElapsedTimeTicks();
        }

        /// Returns the last captured elapsed ticks.
        /// @return Elapsed ticks. Before destruction this is the original destination value.
        inline Ticks getElapsedTimeTicks() const
        {
            return ticks;
        }
    };
}
