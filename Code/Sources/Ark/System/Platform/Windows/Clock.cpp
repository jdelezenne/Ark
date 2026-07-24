#if defined(ARK_PLATFORM_WINDOWS)

#include "Ark/System/Clock.hpp"
#include "Ark/Core/Time.hpp"

#include <Windows.h>

namespace Ark::System
{
    namespace
    {
        static Ticks frequency = 0;
    }

    Outcome Clock::initialize()
    {
        LARGE_INTEGER freq{};
        if (QueryPerformanceFrequency(&freq) == 0)
        {
            return Outcome(unexpectedResult);
        }

        frequency = static_cast<Ticks>(freq.QuadPart);
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
        LARGE_INTEGER counter{};
        QueryPerformanceCounter(&counter);
        return static_cast<Ticks>(counter.QuadPart);
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

#endif
