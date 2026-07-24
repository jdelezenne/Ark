#include "Ark/System/Clock.hpp"
#include "Ark/Core/Time.hpp"

#include <ctime>

namespace Ark::System
{
    namespace
    {
        static Ticks frequency = static_cast<Ticks>(TimeHelper::nanosecondsPerSecond);
    }

    Outcome Clock::initialize()
    {
        return Outcome();
    }

    Ticks Clock::getFrequency()
    {
        return frequency;
    }

    Ticks Clock::getTicks()
    {
        timespec ts{};
        clock_gettime(CLOCK_MONOTONIC, &ts);
        unsigned long long const nanos = static_cast<unsigned long long>(ts.tv_sec) * static_cast<unsigned long long>(TimeHelper::nanosecondsPerSecond) + static_cast<unsigned long long>(ts.tv_nsec);
        return static_cast<Ticks>(nanos);
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
