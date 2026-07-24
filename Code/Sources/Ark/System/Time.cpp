#include "Ark/System/Time.hpp"

#include "Ark/Core/DateTime.hpp"

#include <chrono>
#include <thread>

namespace Ark::System::Time
{
    Ark::Result<uint64> getCurrentTimeMilliseconds()
    {
        auto now = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
        auto ms = static_cast<uint64>(now.time_since_epoch().count());
        return Ark::Result<uint64>(ms);
    }

    Ark::Result<uint64> getCurrentTimeNanoseconds()
    {
        auto now = std::chrono::time_point_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now());
        auto ns = static_cast<uint64>(now.time_since_epoch().count());
        return Ark::Result<uint64>(ns);
    }

    int getDaysInMonth(int year, int month)
    {
        return DateTimeConvert::getDaysInMonth(year, month);
    }

    int getDayOfYear(int year, int month, int day)
    {
        return DateTimeConvert::getDayOfYear(year, month, day);
    }

    int getDayOfWeek(int year, int month, int day)
    {
        return DateTimeConvert::getDayOfWeek(year, month, day);
    }

    Ark::Result<uint64> dateTimeToUnixMilliseconds(DateTime const& dateTimeUtc)
    {
        return DateTimeConvert::toUnixMilliseconds(dateTimeUtc);
    }

    Ark::Result<DateTime> unixMillisecondsToDateTime(uint64 unixMs, bool localTime)
    {
        return DateTimeConvert::fromUnixMilliseconds(unixMs, localTime);
    }

    void delay(uint32 milliseconds)
    {
        if (milliseconds == 0)
        {
            std::this_thread::yield();
            return;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    }
}
