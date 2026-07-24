
#include "Ark/System/Clock.hpp"
#include "Ark/Core/Time.hpp"

#include <mach/mach_time.h>

namespace Ark::System
{
    namespace
    {
        static Ticks frequency = 0;
        static mach_timebase_info_data_t timebaseInfo{};
    }

    Outcome Clock::initialize()
    {
        if (timebaseInfo.denom == 0)
        {
            mach_timebase_info(&timebaseInfo);
        }

        unsigned long long const nsPerSecond = static_cast<unsigned long long>(TimeHelper::nanosecondsPerSecond);
        unsigned long long const ticksPerSecond = (nsPerSecond * timebaseInfo.denom) / timebaseInfo.numer;
        frequency = static_cast<Ticks>(ticksPerSecond);
        return Outcome();
    }

    Ticks Clock::getFrequency()
    {
        if (frequency == 0)
        {
            Outcome const initialized = Clock::initialize();
            if (!initialized)
            {
                return 0;
            }
        }

        return frequency;
    }

    Ticks Clock::getTicks()
    {
        return static_cast<Ticks>(mach_absolute_time());
    }

    float64 Clock::getTimeSeconds()
    {
        float64 const ticks = static_cast<float64>(Clock::getTicks());
        float64 const freq = static_cast<float64>(Clock::getFrequency());
        return (freq > 0.0 ? ticks / freq : 0.0);
    }

    float64 Clock::getTimeMilliseconds()
    {
        return Clock::getTimeSeconds() * 1000.0;
    }
}
