#include "Ark/System/Clock.hpp"
#include "Ark/Core/Time.hpp"

#include <chrono>

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
        auto const now = std::chrono::steady_clock::now().time_since_epoch();
        auto const nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();
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
