#pragma once

#include "Ark/Core/Result.hpp"
#include "Ark/System/DateTime.hpp"

namespace Ark::System::Time
{
    /// Gets the current realtime clock in milliseconds since Unix epoch (UTC).
    Ark::Result<uint64> getCurrentTimeMilliseconds();

    /// Gets the current realtime clock in nanoseconds since Unix epoch (UTC).
    Ark::Result<uint64> getCurrentTimeNanoseconds();

    /// Converts a DateTime (interpreted as UTC) to milliseconds since Unix epoch.
    Ark::Result<uint64> dateTimeToUnixMilliseconds(DateTime const& dateTimeUtc);

    /// Converts milliseconds since Unix epoch (UTC) to DateTime (UTC or local).
    Ark::Result<DateTime> unixMillisecondsToDateTime(uint64 unixMs, bool localTime);

    /// Returns number of days in the specified month of a given year, or 0 on invalid input.
    int getDaysInMonth(int year, int month);

    /// Returns day of year [0-365] for the specified date, or -1 on invalid input.
    int getDayOfYear(int year, int month, int day);

    /// Returns day of week [0-6] with 0=Sunday for the specified date, or -1 on invalid input.
    int getDayOfWeek(int year, int month, int day);

    /// Sleep for the specified number of milliseconds.
    void delay(uint32 milliseconds);
}
