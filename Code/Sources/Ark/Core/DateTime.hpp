#pragma once

#include "Ark/Core/Core.hpp"

namespace Ark
{
    /// Day of week enumeration.
    enum class Weekday
    {
        Sunday,
        Monday,
        Tuesday,
        Wednesday,
        Thursday,
        Friday,
        Saturday,
    };

    /// Month enumeration.
    enum class Month
    {
        January = 1,
        February,
        March,
        April,
        May,
        June,
        July,
        August,
        September,
        October,
        November,
        December,
    };

    /// Calendar date representation.
    struct Date final
    {
        uint32 Year{0};
        Month Month{Month::January};
        uint32 Day{0};
        Weekday Weekday{Weekday::Sunday};
    };

    /// Clock time representation.
    struct Time final
    {
        uint32 Hour{0};
        uint32 Minute{0};
        uint32 Second{0};
        uint32 Millisecond{0};
    };

    /// Combined date and time value.
    struct DateTime final
    {
        Date Date;
        Time Time;
    };

    namespace DateTimeConvert
    {
        /// Returns number of days in the specified month of a given year, or 0 on invalid input.
        int getDaysInMonth(int year, int month);

        /// Returns day of year [0-365] for the specified date, or -1 on invalid input.
        int getDayOfYear(int year, int month, int day);

        /// Returns day of week [0-6] with 0=Sunday for the specified date, or -1 on invalid input.
        int getDayOfWeek(int year, int month, int day);

        /// Converts a DateTime (interpreted as UTC) to milliseconds since Unix epoch.
        Result<uint64> toUnixMilliseconds(DateTime const& dateTimeUtc);

        /// Converts milliseconds since Unix epoch to DateTime.
        /// @param localTime When true, uses the process local timezone; otherwise UTC.
        Result<DateTime> fromUnixMilliseconds(uint64 unixMs, bool localTime);
    }
}
