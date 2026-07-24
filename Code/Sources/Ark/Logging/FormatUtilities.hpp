#pragma once

#include "Ark/Core/Result.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Strings/AnsiConversions.hpp"
#include "Ark/Strings/String.hpp"
#include "Ark/Strings/StringSlice.hpp"
#include "Ark/System/Time.hpp"

namespace Ark::Logging::Internal
{
    inline String formatUnsigned(uint64 value)
    {
        return Strings::fromInt(value).getValueOr("0");
    }

    inline void appendPadded(String& output, uint32 value, usize width)
    {
        char digits[16]{};
        usize index = sizeof(digits);
        uint32 remaining = value;
        do
        {
            digits[--index] = static_cast<char>('0' + (remaining % 10));
            remaining /= 10;
        }
        while (remaining != 0);

        usize const length = sizeof(digits) - index;
        for (usize i = length; i < width; ++i)
        {
            output.append('0');
        }
        output.append(StringSlice(digits + index, length));
    }

    inline String formatDateTime(uint64 timestampMs, char dateSeparator, char dateTimeSeparator, bool includeTime)
    {
        Result<DateTime> dateTime = System::Time::unixMillisecondsToDateTime(timestampMs, true);
        if (!dateTime.isOk())
        {
            return formatUnsigned(timestampMs);
        }

        DateTime const& value = dateTime.getValue();
        String output;
        appendPadded(output, value.Date.Year, 4);
        output.append(dateSeparator);
        appendPadded(output, static_cast<uint32>(value.Date.Month), 2);
        output.append(dateSeparator);
        appendPadded(output, value.Date.Day, 2);

        if (includeTime)
        {
            output.append(dateTimeSeparator);
            appendPadded(output, value.Time.Hour, 2);
            output.append(':');
            appendPadded(output, value.Time.Minute, 2);
            output.append(':');
            appendPadded(output, value.Time.Second, 2);
        }

        return output;
    }

    inline String formatCompactDateTime(uint64 timestampMs)
    {
        Result<DateTime> dateTime = System::Time::unixMillisecondsToDateTime(timestampMs, true);
        if (!dateTime.isOk())
        {
            return formatUnsigned(timestampMs);
        }

        DateTime const& value = dateTime.getValue();
        String output;
        appendPadded(output, value.Date.Year, 4);
        appendPadded(output, static_cast<uint32>(value.Date.Month), 2);
        appendPadded(output, value.Date.Day, 2);
        output.append('_');
        appendPadded(output, value.Time.Hour, 2);
        appendPadded(output, value.Time.Minute, 2);
        appendPadded(output, value.Time.Second, 2);
        return output;
    }

    inline uint64 dateTimeToComparable(DateTime const& value)
    {
        return System::Time::dateTimeToUnixMilliseconds(value).getValueOr(0);
    }
}
